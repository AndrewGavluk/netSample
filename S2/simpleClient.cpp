#include <iostream>

#include "olc_net.h"
#include "net_client.h"
#include "CustomMsgTypes.h"

class CustomClient : public olc::net::client_interface<CustomMsgTypes>
{
public:
    void PingServer()	
	{
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::ServerPing;

		// Caution with this...
		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();		

		msg << timeNow;
		Send(msg);
	}

	void MessageAll()
	{
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::MessageAll;
		Send(msg);
	}
};

std::string GetLineFromCin() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

int main(int argc, char** argv)
{
    CustomClient c;
    c.Connect("127.0.0.1", 30000);

    bool key[3] = { false, false, false };
	bool old_key[3] = { false, false, false };

    bool bQuit = false;
    std::string command;

	auto future = std::async(std::launch::async, GetLineFromCin);

	while (!bQuit)
	{
		if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) 
		{
			auto fut = future.get();
			if (fut == "ping")
            		c.PingServer();
			if (fut == "all")
            		c.MessageAll();

			future = std::async(std::launch::async, GetLineFromCin);
		}

        if (c.IsConnected())
		{
			if (!c.Incoming().empty())
			{
                auto msg = c.Incoming().pop_front().msg;
				switch (msg.header.id)
				{
					case CustomMsgTypes::ServerAccept:{
						std::cout << "Server Accepted Connection\n";
					}break;

                    case CustomMsgTypes::ServerPing:
				    {
						// Server has responded to a ping request
						std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
						std::chrono::system_clock::time_point timeThen;
						msg >> timeThen;
						std::cout << "Ping: " << std::chrono::duration<double>(timeNow - timeThen).count() << "\n";
				    }break;

					case CustomMsgTypes::MessageAll :
					{
						uint32_t clientID;
						msg >> clientID;
						std::cout << "Message from[" << clientID << "]\n";
					}break;
                }
            }
        }
        else
        {

        }

    }

    return 0;
}