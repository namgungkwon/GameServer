#pragma once

#include "Protocol.pb.h"
#include "Packet.h"
#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
#include "NetworkHeader.h"
#endif

bool Handle_LC_CREATE_ACCOUNT(PacketSessionRef& session, Protocol::LC_CREATE_ACCOUNT& pkt);
bool Handle_LC_LOGIN(PacketSessionRef& session, Protocol::LC_LOGIN& pkt);

///////////////////////////////////////////////////////
//	<	LoginServerPacketHandler	>
///////////////////////////////////////////////////////
class LoginServerPacketHandler
{
public:
	static void Init()
	{
		GPacketHandler[PKT_LC_CREATE_ACCOUNT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::LC_CREATE_ACCOUNT>(Handle_LC_CREATE_ACCOUNT, session, buffer, len); };
		GPacketHandler[PKT_LC_LOGIN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<Protocol::LC_LOGIN>(Handle_LC_LOGIN, session, buffer, len); };
	}

	static bool	HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}


	static SendBufferRef MakeSendBuffer(Protocol::CL_CREATE_ACCOUNT& pkt) { return MakeSendBuffer(pkt, PKT_CL_CREATE_ACCOUNT); }
	static SendBufferRef MakeSendBuffer(Protocol::CL_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_CL_LOGIN); }
	

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
