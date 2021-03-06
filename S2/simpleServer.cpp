#include <iostream>

#include "net_server.h"
#include "CustomMsgTypes.h"


class CustomServer : public olc::net::server_interface<CustomMsgTypes>
{
public:
	CustomServer(uint16_t nPort) : olc::net::server_interface<CustomMsgTypes>(nPort)
	{

	}

protected:
	bool OnClientConnect(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client)
	{
		return true;
	}

	// Called when a client appears to have disconnected
	void OnClientDisconnect(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client)
	{
		
	}

	// Called when a message arrives
	void OnMessage(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client, const olc::net::message<CustomMsgTypes>& msg) override
	{
		/*switch (msg.header.id)
		{
			case CustomMsgTypes::ServerPing:*/
			{
			std::cout << "[" << client->GetID() << "]: Server Ping\n";
			 //Simply bounce message back to client
			client->Send(msg);
			}
			//break;
		//}

	}
};

int main()
{
	CustomServer server(60000); 
	server.Start();

	while (1)
	{
		server.Update(-1, true);
	}
	
	return 0;
}