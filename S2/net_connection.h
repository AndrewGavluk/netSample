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
            connection();
            ~connection();

			void ConnectToClient(uint32_t uid = 0)
			{
			}

            bool ConnectToServer();
            bool Disconnect();
            bool IsConnect();
            bool Send(const olc::net::message<T> & msg);

        protected:
            tcp::socket m_socket;
            asio::io_context& m_asioContext;
            tsqueue<message<T>> m_qMessagesOut;
            tsqueue<owned_message<T>> m_qMessagesIn;

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
