#pragma once
#include <cstdint>

enum class CustomMsgTypes : uint32_t
{
	ServerAccept = 1,
	ServerDeny = 2,
	ServerPing = 3,
	MessageAll = 4,
	ServerMessage = 5
};