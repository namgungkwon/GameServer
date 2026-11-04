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
	PKT_CL_CREATE_ACCOUNT = 1000,
	PKT_LC_CREATE_ACCOUNT = 1001,
	PKT_CL_LOGIN = 1002,
	PKT_LC_LOGIN = 1003,
	PKT_CG_AUTH_TOKEN = 1004,
	PKT_GC_AUTH_TOKEN = 1005,
	PKT_CG_ENTER_GAME = 1006,
	PKT_GC_ENTER_GAME = 1007,
	PKT_CG_LEAVE_GAME = 1008,
	PKT_GC_SPAWN = 1009,
	PKT_GC_DESPAWN = 1010,
	PKT_CG_REVIVE = 1011,
	PKT_GC_REVIVE = 1012,
	PKT_CG_MOVE = 1013,
	PKT_GC_MOVE = 1014,
	PKT_CG_ATTACK = 1015,
	PKT_GC_ATTACK = 1016,
	PKT_GC_DAMAGE = 1017,
	PKT_GC_REWARD = 1018,
	PKT_GC_LEVELUP = 1019,
	PKT_CG_CHAT = 1020,
	PKT_GC_CHAT = 1021,
};