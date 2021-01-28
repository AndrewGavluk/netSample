#include "net_common.h"

namespace olc
{
    namespace net
    {
        // Message Header is sent at start of all messages. The template allows us
		// to use "enum class" to ensure that the messages are valid at compile time
		template <typename T>
		struct message_header
		{
			T id{};
			uint32_t size = 0;
		};

		// Message Body contains a header and a std::vector, containing raw bytes
		// of infomation. This way the message can be variable length, but the size
		// in the header must be updated.
        template <typename T>
        struct message
        {
            // Header & Body vector
			message_header<T> header{};
			std::vector<uint8_t> body;
            
            // returns size of entire message packet in bytes
            size_t size() const;
            
            // Pushes any POD-like data into the message buffer
            template<typename DataType>
			friend message<T>& operator << (message<T>& msg, const DataType& data);
        };
         

    }
}