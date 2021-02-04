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
        /*if (GetForegroundWindow() == GetConsoleWindow())
		{
			key[0] = GetAsyncKeyState('1') & 0x8000;
			key[1] = GetAsyncKeyState('2') & 0x8000;
			key[2] = GetAsyncKeyState('3') & 0x8000;
		}

		if (key[0] && !old_key[0]) c.PingServer();
		if (key[1] && !old_key[1]) c.MessageAll();
		if (key[2] && !old_key[2]) bQuit = true;*/

        //std::cin >> command;
		try
		{
			if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) 
			{
				if (future.get() == "ping")
            		c.PingServer();

				future = std::async(std::launch::async, GetLineFromCin);
			}
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
		

		//for (int i = 0; i < 3; i++) old_key[i] = key[i];

        if (c.IsConnected())
		{
			if (!c.Incoming().empty())
			{
                auto msg = c.Incoming().pop_front().msg;
				//switch (msg.header.id)
				{
                    //case CustomMsgTypes::ServerPing:
				    //{
					// Server has responded to a ping request
					std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
					std::chrono::system_clock::time_point timeThen;
					msg >> timeThen;
					std::cout << "Ping: " << std::chrono::duration<double>(timeNow - timeThen).count() << "\n";
				    //}
                }
            }
        }
        else
        {

        }
		//std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}