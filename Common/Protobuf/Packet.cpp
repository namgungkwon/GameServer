#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING == 0
#include "pch.h"
#endif
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