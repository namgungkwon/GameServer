#pragma once

namespace Config
{
	static constexpr uint32 KB = 1024;
	static constexpr uint32 MB = 1024 * KB;

	namespace Packet
	{
		static constexpr uint32 MAX_PACKET_SIZE = 2 * KB;
	}

	namespace Session
	{
		static constexpr uint32 RECV_BUFFER_SIZE = 4 * KB;
	}

	namespace Buffer
	{
		static constexpr uint32 SEND_BUFFER_CHUNK_SIZE = 4 * KB;
	}
}