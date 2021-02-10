#include <iostream>

#include "olc_net.h"
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
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::ServerAccept;
		client->Send(msg);
		return true;
	}

	// Called when a client appears to have disconnected
	void OnClientDisconnect(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client)
	{ 
		std::cout << "Rem. client [" << client->GetID() << "]\n";
	}

	// Called when a message arrives
	void OnMessage(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client, const olc::net::message<CustomMsgTypes>& msg) override
	{
		switch (msg.header.id)
		{
			case CustomMsgTypes::ServerPing:
			{
			std::cout << "[" << client->GetID() << "]: Server Ping\n";

			// Simply bounce message back to client
			client->Send(msg);
			}break;

			case CustomMsgTypes::MessageAll:
			{
				std::cout << "[" << client->GetID() << "]: Server Ping\n";
				olc::net::message<CustomMsgTypes> msg;
				msg.header.id = CustomMsgTypes::MessageAll;
				msg << client->GetID();
				SendMessageAll(msg, client);
			}
			break;
		}

	}
};

int main()
{
	CustomServer server(30000); 
	server.Start();

	while (1)
	{
		server.Update();
	}
	
	return 0;
}