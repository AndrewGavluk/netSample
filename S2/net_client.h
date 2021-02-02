#include "net_connection.h"

namespace olc
{
    namespace net
    {
    
        template <typename T>
        class client_interface
        {
        
        public:
            client_interface() = default;

			virtual ~client_interface()
			{
				// If the client is destroyed, always try and disconnect from server
				Disconnect();
			}
            bool Connect(const std::string& host, const uint16_t port)
            {
                try
                {
                    // Resolve hostname/ip-address int tangiable pisical address
                    tcp::resolver resolver(m_context);
                    auto endpoints = resolver.resolve(host, std::to_string(port)); // asio::ip::tcp::resolver::results_type

                    m_connection = std::make_unique<connection<T>>(connection<T>::owner::client, m_context, asio::ip::tcp::socket(m_context), m_qMessagesIn);

                    // Tell connectioin connect to server
                    m_connection->ConnectToServer(endpoints);

                    // Start Context Thread
					thrContext = std::thread([this]() { m_context.run(); });
					
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
                    m_connection->Disconnect();

                m_context.stop();

                if (thrContext.joinable())
                    thrContext.join();

                m_connection.release();

            }

            bool IsConnected()
            {
                if (m_connection)
                    return m_connection->IsConnect();
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

        protected:
            asio::io_context m_context;
            std::thread thrContext;
            std::unique_ptr<connection<T>> m_connection;

        private:
            tsqueue<owned_message<T>> m_qMessagesIn;
        };

    }
}