#pragma once
#include "net_tsqueue.h"
#include "net_message.h"

namespace olc
{
	namespace net
	{
        template <typename T>
		struct owned_message;

        template<typename T>
		class connection : public std::enable_shared_from_this<connection<T>>
		{
		public:

			enum class owner
			{
			server,
			client
			};

            connection(owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, tsqueue<owned_message<T>>& qIn)
				: m_asioContext(asioContext), m_socket(std::move(socket)), m_qMessagesIn(qIn)
			{
				m_nOwnerType = parent;
			}

            virtual ~connection(){};

		// This ID is used system wide - its how clients will understand other clients
			// exist across the whole system.
			uint32_t GetID() const
			{
				return id;
			}

			void ConnectToClient(uint32_t uid = 0)
			{
				if (m_nOwnerType == owner::server)
				{
					if (m_socket.is_open())
					{
						id = uid;
						ReadHeader();
					}
				}
			}

            bool ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints)
			{
				if (m_nOwnerType == owner::client)
				{
					// Request asio attempts to connect to an endpoint
					asio::async_connect(m_socket, endpoints,
						[this](std::error_code ec, asio::ip::tcp::endpoint endpoint)
						{
							if (!ec)
							{
								ReadHeader();
							}
						});
				}
			}

            bool Disconnect()
			{
				if (IsConnect())
					asio::post(m_asioContext, [this]() { m_socket.close(); });
			}

            bool IsConnect() const
			{
				return m_socket.is_open();
			}

            bool Send(const olc::net::message<T> & msg)
			{
				asio::post(m_asioContext,
					[this, msg]()
					{
						bool bWritingMessage = !m_qMessagesOut.empty();
						m_qMessagesOut.push_back(msg);
						if (!bWritingMessage)
						{
							WriteHeader();
						}
					});
			}

		private:

			// ASYNC - Wite a message header
			void WriteHeader()
			{
				asio::async_write(m_socket, asio::buffer(&m_qMessagesOut.front().header, sizeof(message_header<T>)),
					[this](std::error_code ec, std::size_t length){
						if (!ec)
						{
							if (m_qMessagesOut.front().body.size() > 0)
								WriteBody();
							else
							{
								m_qMessagesOut.pop_front();
								if (!m_qMessagesOut.empty())
									WriteHeader();
							}
						}
						else
						{
							std::cout << "[" << id << "] Write Header Fail.\n";
							m_socket.close();
						}
					});
			}

			// ASYNC - Write a message body
			void WriteBody()
			{
				asio::async_write(m_socket, asio::buffer(m_qMessagesOut.front().body.data(), m_qMessagesOut.front().body.size()),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							m_qMessagesOut.pop_front();
							if (!m_qMessagesOut.empty())
								WriteHeader();
						}
						else
						{
							std::cout << "[" << id << "] Write Body Fail.\n";
							m_socket.close();
						}
					});
			}


			// ASYNC - Read a message header
			void ReadHeader()
			{
				asio::async_read(m_socket, asio::buffer(&m_msgTemporaryIn.header, sizeof(message_header<T>)),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							if (m_msgTemporaryIn.header.size > 0)
							{
								m_msgTemporaryIn.body.resize(m_msgTemporaryIn.header.size);
								ReadBody();
							}
							else
								AddToIncomingQueue();
						}
						else
						{
							std::cout << "[" << id << "] Read Header Fail.\n";
							m_socket.close();
						}
					});
			}

			// ASYNC - Prime context ready to read a message body
			void ReadBody()
			{
				asio::async_read(m_socket, asio::buffer(&m_msgTemporaryIn.header, sizeof(message_header<T>)),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
							AddToIncomingQueue();
						else
						{
							std::cout << "[" << id << "] Read Body Fail.\n";
							m_socket.close();
						}
					});
			}

			// Once a full message is received, add it to the incoming queue
			void AddToIncomingQueue()
			{
				if(m_nOwnerType == owner::server)
					m_qMessagesIn.push_back({ this->shared_from_this(), m_msgTemporaryIn });
				else
					m_qMessagesIn.push_back({ nullptr, m_msgTemporaryIn });
				
				ReadHeader();
			}

        protected:
            tcp::socket m_socket;
            asio::io_context& m_asioContext;
            tsqueue<message<T>> m_qMessagesOut;
            tsqueue<owned_message<T>>& m_qMessagesIn;
			
			// Incoming messages are constructed asynchronously, so we will
			// store the part assembled message here, until it is ready
			message<T> m_msgTemporaryIn;

			// The "owner" decides how some of the connection behaves
			owner m_nOwnerType = owner::server;

			// Clients will be identified in the "wider system" via an ID
			uint32_t id = 0;

        };

        template <typename T>
		struct owned_message
		{
			std::shared_ptr<connection<T>> remote = nullptr;
			message<T> msg;

			// Again, a friendly string maker
			friend std::ostream& operator<<(std::ostream& os, const owned_message<T>& msg)
			{
				os << msg.msg;
				return os;
			}
		};	
    }
}
