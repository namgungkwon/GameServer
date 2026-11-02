#pragma once

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
#include "NetworkHeader.h"
#include <functional>
#endif

using	PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern	PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
void Init_PacketHandler();

// 패킷 리스트
enum : uint16
{
	PKT_CG_ENTER_GAME = 1000,
	PKT_GC_ENTER_GAME = 1001,
	PKT_CG_LEAVE_GAME = 1002,
	PKT_GC_SPAWN = 1003,
	PKT_GC_DESPAWN = 1004,
	PKT_CG_REVIVE = 1005,
	PKT_GC_REVIVE = 1006,
	PKT_CG_MOVE = 1007,
	PKT_GC_MOVE = 1008,
	PKT_CG_ATTACK = 1009,
	PKT_GC_ATTACK = 1010,
	PKT_GC_DAMAGE = 1011,
};