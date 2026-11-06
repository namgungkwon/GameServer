#pragma once

#include "Protocol.pb.h"
#include "Packet.h"
#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
#include "NetworkHeader.h"
#endif

bool Handle_GC_AUTH_TOKEN(PacketSessionRef& session, Protocol::GC_AUTH_TOKEN& pkt);
bool Handle_GC_CREATE_CHARACTER(PacketSessionRef& session, Protocol::GC_CREATE_CHARACTER& pkt);
bool Handle_GC_ENTER_GAME(PacketSessionRef& session, Protocol::GC_ENTER_GAME& pkt);
bool Handle_GC_SPAWN(PacketSessionRef& session, Protocol::GC_SPAWN& pkt);
bool Handle_GC_DESPAWN(PacketSessionRef& session, Protocol::GC_DESPAWN& pkt);
bool Handle_GC_REVIVE(PacketSessionRef& session, Protocol::GC_REVIVE& pkt);
bool Handle_GC_MOVE(PacketSessionRef& session, Protocol::GC_MOVE& pkt);
bool Handle_GC_ATTACK(PacketSessionRef& session, Protocol::GC_ATTACK& pkt);
bool Handle_GC_DAMAGE(PacketSessionRef& session, Protocol::GC_DAMAGE& pkt);
bool Handle_GC_REWARD(PacketSessionRef& session, Protocol::GC_REWARD& pkt);
bool Handle_GC_LEVELUP(PacketSessionRef& session, Protocol::GC_LEVELUP& pkt);
bool Handle_GC_CHAT(PacketSessionRef& session, Protocol::GC_CHAT& pkt);

///////////////////////////////////////////////////////
//	<	GameServerPacketHandler	>
///////////////////////////////////////////////////////
class GameServerPacketHandler
{
public:
	static void Init()
	{
		GPacketHandler[PKT_GC_AUTH_TOKEN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::GC_AUTH_TOKEN>(Handle_GC_AUTH_TOKEN, session, buffer, len); };
		GPacketHandler[PKT_GC_CREATE_CHARACTER] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::GC_CREATE_CHARACTER>(Handle_GC_CREATE_CHARACTER, session, buffer, len); };
		GPacketHandler[PKT_GC_ENTER_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::GC_ENTER_GAME>(Handle_GC_ENTER_GAME, session, buffer, len); };
		GPacketHandler[PKT_GC_SPAWN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::GC_SPAWN>(Handle_GC_SPAWN, session, buffer, len); };
		GPacketHandler[PKT_GC_DESPAWN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::GC_DESPAWN>(Handle_GC_DESPAWN, session, buffer, len); };
		GPacketHandler[PKT_GC_REVIVE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::GC_REVIVE>(Handle_GC_REVIVE, session, buffer, len); };
		GPacketHandler[PKT_GC_MOVE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::GC_MOVE>(Handle_GC_MOVE, session, buffer, len); };
		GPacketHandler[PKT_GC_ATTACK] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::GC_ATTACK>(Handle_GC_ATTACK, session, buffer, len); };
		GPacketHandler[PKT_GC_DAMAGE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::GC_DAMAGE>(Handle_GC_DAMAGE, session, buffer, len); };
		GPacketHandler[PKT_GC_REWARD] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::GC_REWARD>(Handle_GC_REWARD, session, buffer, len); };
		GPacketHandler[PKT_GC_LEVELUP] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::GC_LEVELUP>(Handle_GC_LEVELUP, session, buffer, len); };
		GPacketHandler[PKT_GC_CHAT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::GC_CHAT>(Handle_GC_CHAT, session, buffer, len); };
	}

	static bool	HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}


	static SendBufferRef MakeSendBuffer(Protocol::CG_AUTH_TOKEN& pkt) { return MakeSendBuffer(pkt, PKT_CG_AUTH_TOKEN); }
	static SendBufferRef MakeSendBuffer(Protocol::CG_CREATE_CHARACTER& pkt) { return MakeSendBuffer(pkt, PKT_CG_CREATE_CHARACTER); }
	static SendBufferRef MakeSendBuffer(Protocol::CG_ENTER_GAME& pkt) { return MakeSendBuffer(pkt, PKT_CG_ENTER_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::CG_LEAVE_GAME& pkt) { return MakeSendBuffer(pkt, PKT_CG_LEAVE_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::CG_REVIVE& pkt) { return MakeSendBuffer(pkt, PKT_CG_REVIVE); }
	static SendBufferRef MakeSendBuffer(Protocol::CG_MOVE& pkt) { return MakeSendBuffer(pkt, PKT_CG_MOVE); }
	static SendBufferRef MakeSendBuffer(Protocol::CG_ATTACK& pkt) { return MakeSendBuffer(pkt, PKT_CG_ATTACK); }
	static SendBufferRef MakeSendBuffer(Protocol::CG_CHAT& pkt) { return MakeSendBuffer(pkt, PKT_CG_CHAT); }
	

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
		SendBufferRef sendBuffer = MakeShared<SendBuffer>(packetSize);
#else
		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
#endif

		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		pkt.SerializeToArray(&header[1], dataSize);
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};
