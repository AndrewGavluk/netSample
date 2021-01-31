#include "net_tsqueue.h"
#include "net_message.h"

namespace olc
{
	namespace net
	{
        template<typename T>
		class connection : public std::enable_shared_from_this<connection<T>>
		{
		public:
            connection();
            ~connection();
            bool ConnectToServer();
            bool Disconnect();
            bool IsConnect();
            bool Send(const olc::net::message<T> & msg);

        protected:
            asio::ip::tcp::socket m_socket;
            asio::io_context& m_asioContext;
            tsqueue<message<T>> m_qMessagesOut;
            tsqueue<owned_message<T>> m_qMessagesIn;

        };
    }
}