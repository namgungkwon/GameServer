#include "pch.h"
#include "Packet.h"


PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}


void Init_PacketHandler()
{
	for (int32 i = 0; i < UINT16_MAX; ++i)
		GPacketHandler[i] = Handle_INVALID;
}