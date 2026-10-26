#pragma once
class AISystem
{
public:
	static void Update(ZoneRef zone, uint64 nowTick, uint64 deltaTime, OUT Vector<EntityRef>& movingEntity);
};