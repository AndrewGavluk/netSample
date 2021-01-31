#include "net_connection.h"

namespace olc
{
    namespace net
    {
    
        template <typename T>
        class client_interface
        {
        
        public:
            client_interface() 
			{}

			virtual ~client_interface()
			{
				// If the client is destroyed, always try and disconnect from server
				Disconnect();
			}
            bool Connect(const std::string& host, const uint16_t port)
            {
                try
                {
                    m_connectioin = std::make_unique<connection<T>>{};

                    // Resolve hostname/ip-address int tangiable pisical address
                    asio::ip::tcp::resolver resolver(m_context);
                    auto endpoints = resolver.resolve(host, std::to_string(port)); // asio::ip::tcp::resolver::results_type


                    // Tell connectioin connect to server
                    m_connectioin->ConnectToServer(endpoints)
					
                }
                catch(const std::exception& e)
                {
                    std::cerr << "Client exception:" << e.what() << '\n';
                    return false;
                }
                

                return false;
            }

            void Disconnect()
            {
                if (IsConnected())
                    m_connectioin->Disconnect();

                m_context.stop();

                if (thrContext.joinable())
                    thrContext.join();

                m_connectioin.release();

            }

            bool IsConnected()
            {
                if (m_connectioin)
                    return m_connectioin->IsConnected();
                else
                    return false;
            }

            // Send message to server
			void Send(const message<T>& msg)
			{
				if (IsConnected())
					 m_connection->Send(msg);
			}

			// Retrieve queue of messages from server
			tsqueue<owned_message<T>>& Incoming()
			{ 
				return m_qMessagesIn;
			}

            tsqueue<owned_message<T>>& Incoming()
            {
                return m_qMessagesIn;
            }

        protected:
            asio::io_context m_context;
            std::thread thrContext;
            asio::ip::tcp::socket m_socket;
            std::unique_ptr<connection<T>> m_connectioin;

        private:
            tsqueue<owned_message<T>> m_qMessagesIn;
        };

    }
}