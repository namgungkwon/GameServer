#pragma once


///////////////////////////////////////////////////////
//	<	AISystem	>
///////////////////////////////////////////////////////
class AISystem
{
public:
	static void			Update(ZoneRef zone, uint64 nowTick, uint64 deltaTime, OUT Vector<EntityRef>& movingEntity);
	static bool			UpdateAIMove(NPCRef npc, uint64 nowTick, uint64 deltaTime);
};