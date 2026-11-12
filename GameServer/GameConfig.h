#pragma once


///////////////////////////////////////////////////////
//	<	GameConfig	>
///////////////////////////////////////////////////////
namespace Config
{
	namespace Packet
	{
		static constexpr uint32 SPAWNPACKET_BYTES = 256;
		static constexpr uint32 DESPAWNPACKET_BYTES = 10;
		static constexpr uint32 MOVEPACKET_BYTES = 80; 

		static constexpr uint32 MAXCOUNT_SPAWNPKT = (Config::Buffer::SEND_BUFFER_CHUNK_SIZE - sizeof(PacketHeader)) / SPAWNPACKET_BYTES;
		static constexpr uint32 MAXCOUNT_DESPAWNPKT = (Config::Buffer::SEND_BUFFER_CHUNK_SIZE - sizeof(PacketHeader)) / DESPAWNPACKET_BYTES;
		static constexpr uint32 MAXCOUNT_MOVEPKT = (Config::Buffer::SEND_BUFFER_CHUNK_SIZE - sizeof(PacketHeader)) / MOVEPACKET_BYTES;
	}

	namespace World
	{
		static constexpr uint64 WORLD_UPDATE_TICK = 50;
	}

	namespace AI
	{
		static constexpr uint64 PLAYER_FIND_TICK = 500;
		static constexpr uint64 PATH_FIND_TICK = 500;
	}

	namespace Player
	{
		static constexpr float pos_x = 50000.f;
		static constexpr float pos_y = 50000.f;
		static constexpr float pos_z = 0.f;

		static constexpr uint64 SEND_MOVE_TERM = 500;
	}
};