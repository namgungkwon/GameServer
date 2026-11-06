#pragma once

#include "Protocol.pb.h"
#include "Packet.h"
#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
#include "NetworkHeader.h"
#endif

bool Handle_CG_AUTH_TOKEN(PacketSessionRef& session, Protocol::CG_AUTH_TOKEN& pkt);
bool Handle_CG_CREATE_CHARACTER(PacketSessionRef& session, Protocol::CG_CREATE_CHARACTER& pkt);
bool Handle_CG_ENTER_GAME(PacketSessionRef& session, Protocol::CG_ENTER_GAME& pkt);
bool Handle_CG_LEAVE_GAME(PacketSessionRef& session, Protocol::CG_LEAVE_GAME& pkt);
bool Handle_CG_REVIVE(PacketSessionRef& session, Protocol::CG_REVIVE& pkt);
bool Handle_CG_MOVE(PacketSessionRef& session, Protocol::CG_MOVE& pkt);
bool Handle_CG_ATTACK(PacketSessionRef& session, Protocol::CG_ATTACK& pkt);
bool Handle_CG_CHAT(PacketSessionRef& session, Protocol::CG_CHAT& pkt);

///////////////////////////////////////////////////////
//	<	ClientPacketHandler	>
///////////////////////////////////////////////////////
class ClientPacketHandler
{
public:
	static void Init()
	{
		GPacketHandler[PKT_CG_AUTH_TOKEN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::CG_AUTH_TOKEN>(Handle_CG_AUTH_TOKEN, session, buffer, len); };
		GPacketHandler[PKT_CG_CREATE_CHARACTER] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::CG_CREATE_CHARACTER>(Handle_CG_CREATE_CHARACTER, session, buffer, len); };
		GPacketHandler[PKT_CG_ENTER_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::CG_ENTER_GAME>(Handle_CG_ENTER_GAME, session, buffer, len); };
		GPacketHandler[PKT_CG_LEAVE_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::CG_LEAVE_GAME>(Handle_CG_LEAVE_GAME, session, buffer, len); };
		GPacketHandler[PKT_CG_REVIVE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::CG_REVIVE>(Handle_CG_REVIVE, session, buffer, len); };
		GPacketHandler[PKT_CG_MOVE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::CG_MOVE>(Handle_CG_MOVE, session, buffer, len); };
		GPacketHandler[PKT_CG_ATTACK] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::CG_ATTACK>(Handle_CG_ATTACK, session, buffer, len); };
		GPacketHandler[PKT_CG_CHAT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::CG_CHAT>(Handle_CG_CHAT, session, buffer, len); };
	}

	static bool	HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}


	static SendBufferRef MakeSendBuffer(Protocol::GC_AUTH_TOKEN& pkt) { return MakeSendBuffer(pkt, PKT_GC_AUTH_TOKEN); }
	static SendBufferRef MakeSendBuffer(Protocol::GC_CREATE_CHARACTER& pkt) { return MakeSendBuffer(pkt, PKT_GC_CREATE_CHARACTER); }
	static SendBufferRef MakeSendBuffer(Protocol::GC_ENTER_GAME& pkt) { return MakeSendBuffer(pkt, PKT_GC_ENTER_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::GC_SPAWN& pkt) { return MakeSendBuffer(pkt, PKT_GC_SPAWN); }
	static SendBufferRef MakeSendBuffer(Protocol::GC_DESPAWN& pkt) { return MakeSendBuffer(pkt, PKT_GC_DESPAWN); }
	static SendBufferRef MakeSendBuffer(Protocol::GC_REVIVE& pkt) { return MakeSendBuffer(pkt, PKT_GC_REVIVE); }
	static SendBufferRef MakeSendBuffer(Protocol::GC_MOVE& pkt) { return MakeSendBuffer(pkt, PKT_GC_MOVE); }
	static SendBufferRef MakeSendBuffer(Protocol::GC_ATTACK& pkt) { return MakeSendBuffer(pkt, PKT_GC_ATTACK); }
	static SendBufferRef MakeSendBuffer(Protocol::GC_DAMAGE& pkt) { return MakeSendBuffer(pkt, PKT_GC_DAMAGE); }
	static SendBufferRef MakeSendBuffer(Protocol::GC_REWARD& pkt) { return MakeSendBuffer(pkt, PKT_GC_REWARD); }
	static SendBufferRef MakeSendBuffer(Protocol::GC_LEVELUP& pkt) { return MakeSendBuffer(pkt, PKT_GC_LEVELUP); }
	static SendBufferRef MakeSendBuffer(Protocol::GC_CHAT& pkt) { return MakeSendBuffer(pkt, PKT_GC_CHAT); }
	

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
