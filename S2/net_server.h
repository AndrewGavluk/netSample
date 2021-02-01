#include "net_connection.h"
#include "net_message.h"

namespace olc 
{
    namespace net
    {
        template <typename T>
        class server_interface
        {
        public:
            server_interface(uint16_t port) 
                : m_asioAcceptor(m_asio_context, asio::ip::tcp::endpoint( asio::ip::tcp::v4(), port))
            {
                
            }

            virtual ~server_interface()
            {
                Stop();
            }

            bool Start()
            {
                try
                {
                    ClientWait();
                    m_threadContext = std::thread([this] () {m_asio_context.run();});
                }
                catch(const std::exception& e)
                {
                    std::cerr << "[SERVER] Exception:" <<e.what() << '\n';
                    return false;
                }
                
                std::cout << "[SERVER] Started!\n" <<'\n';
                return true;
            }

            void Stop()
            {
                m_asio_context.stop();
                m_threadContext.joinable() ? m_threadContext.join() : (void)(m_threadContext);
                std::cout << "[SERVER] Stopped!\n";
            }

            // async
            void ClientWait()
            {
                m_asioAcceptor.async_accept(
                    [this](std::error_code ec, tcp::socket socket)
                    {
                        if (!ec)
                        {
                            std::cout << "[SERVER] New connection: " << socket.remote_endpoint() << "\n";
                            /*std::shared_ptr<connection<T>> newConnection (connection<T>::owner::server, m_asio_context, std::move(socket), m_qMessagesIn);
                        
                            if (OnClientConnect(newConnection))
                            {
                                m_deqConnections.push_back(std::move(newConnection));
                                m_deqConnections.back()->ConnectToClient(nIdCounter++);
                                std::cout << "[" << m_deqConnections.back()->GetID() << "] Connection Approved\n";
                            }*/
                        }
                        else    
                            std::cout << "[SERVER] New connectioon error: " << ec.message() << "\n";
                        ClientWait(); // Recursion overflow ???
                   }
                );
            }

            // send message a client
            void SendMessage(std::shared_ptr<connection<T>> Client, const message<T>& msg)
            {
                if (Client && Client->IsConnect())
                    Client->Send(msg);
                else
                {
                    OnClientDisconnect(Client);
                    Client.reset();
                    m_deqConnections.erase(
                        std::remove(m_deqConnections.begin(), m_deqConnections.end(), m_deqConnections.end()));
                }
            }

            void SendMessageAll(const message<T>& msg, std::shared_ptr<connection<T>> ignClient = nullptr)
            {
                bool bInvalidClientExists = false;

                for (auto& client : m_deqConnections)
                    if (client && client->IsConnected())
                    {
                        if (client != ignClient)
                            client->Send(msg);
                    }
                    else
                    {
                        OnClientDisconnect(client);
                        client.reset();
                    }
                
                // Remove dead clients, all in one go - this way, we dont invalidate the
				// container as we iterated through it.
				if (bInvalidClientExists)
					m_deqConnections.erase(
						std::remove(m_deqConnections.begin(), m_deqConnections.end(), nullptr), m_deqConnections.end());
            }

			// Force server to respond to incoming messages
			void Update(size_t nMaxMessages = -1, bool bWait = false)
			{
				if (bWait) m_qMessagesIn.wait();

				// Process as many messages as you can up to the value
				// specified
				size_t nMessageCount = 0;
				while (nMessageCount < nMaxMessages && !m_qMessagesIn.empty())
				{
					// Grab the front message
					auto msg = m_qMessagesIn.pop_front();

					// Pass to message handler
					OnMessage(msg.remote, msg.msg);

					nMessageCount++;
				}
			}
        protected:
            // to deny the client connection
            virtual bool OnClientConnect(std::shared_ptr<connection<T>> Client)
            {
                return false;
            }

            virtual void OnClientDisconnect(std::shared_ptr<connection<T>> Client)
            {

            }

            virtual void OnMessage(std::shared_ptr<connection<T>> Client, const message<T>& msg)
            {

            }

        protected:

            tsqueue<owned_message<T>> m_qMessagesIn;

            std::deque<std::shared_ptr<connection<T>>> m_deqConnections;

            asio::io_context m_asio_context;
            std::thread m_threadContext;

            tcp::acceptor m_asioAcceptor;

            uint32_t nIdCounter = 10000;
            uint32_t id = 0;
        };
    }
}