#pragma once



namespace Config
{
	namespace Packet
	{
		static constexpr uint32 DESPAWNPACKET_BYTES = 8; // objectId
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
};