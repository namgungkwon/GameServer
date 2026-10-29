#pragma once

#include "Protocol.pb.h"
#include "Packet.h"
#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
#include "NetworkHeader.h"
#endif

bool Handle_GC_ENTER_GAME(PacketSessionRef& session, Protocol::GC_ENTER_GAME& pkt);
bool Handle_GC_SPAWN(PacketSessionRef& session, Protocol::GC_SPAWN& pkt);
bool Handle_GC_DESPAWN(PacketSessionRef& session, Protocol::GC_DESPAWN& pkt);
bool Handle_GC_MOVE(PacketSessionRef& session, Protocol::GC_MOVE& pkt);
bool Handle_GC_ATTACK(PacketSessionRef& session, Protocol::GC_ATTACK& pkt);
bool Handle_GC_DAMAGE(PacketSessionRef& session, Protocol::GC_DAMAGE& pkt);

///////////////////////////////////////////////////////
//	<	GameServerPacketHandler	>
///////////////////////////////////////////////////////
class GameServerPacketHandler
{
public:
	static void Init()
	{
		GPacketHandler[PKT_GC_ENTER_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::GC_ENTER_GAME>(Handle_GC_ENTER_GAME, session, buffer, len); };
		GPacketHandler[PKT_GC_SPAWN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::GC_SPAWN>(Handle_GC_SPAWN, session, buffer, len); };
		GPacketHandler[PKT_GC_DESPAWN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::GC_DESPAWN>(Handle_GC_DESPAWN, session, buffer, len); };
		GPacketHandler[PKT_GC_MOVE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::GC_MOVE>(Handle_GC_MOVE, session, buffer, len); };
		GPacketHandler[PKT_GC_ATTACK] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::GC_ATTACK>(Handle_GC_ATTACK, session, buffer, len); };
		GPacketHandler[PKT_GC_DAMAGE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::GC_DAMAGE>(Handle_GC_DAMAGE, session, buffer, len); };
	}

	static bool	HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}


	static SendBufferRef MakeSendBuffer(Protocol::CG_ENTER_GAME& pkt) { return MakeSendBuffer(pkt, PKT_CG_ENTER_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::CG_LEAVE_GAME& pkt) { return MakeSendBuffer(pkt, PKT_CG_LEAVE_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::CG_MOVE& pkt) { return MakeSendBuffer(pkt, PKT_CG_MOVE); }
	static SendBufferRef MakeSendBuffer(Protocol::CG_ATTACK& pkt) { return MakeSendBuffer(pkt, PKT_CG_ATTACK); }
	

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
