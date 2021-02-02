#pragma once
#include <cstdint>

enum class CustomMsgTypes : uint8_t
{
	ServerAccept,
	ServerDeny,
	ServerPing,
	MessageAll,
	ServerMessage
};