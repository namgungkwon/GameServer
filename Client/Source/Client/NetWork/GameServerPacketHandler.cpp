#include "GameServerPacketHandler.h"

#include "NetworkManager.h"
#include "Client/Client.h"
#include "Client/ObjectHandler.h"

UObjectHandler* GetObjectHandler(PacketSessionRef session)
{
	for (auto World : GEngine->GetWorldContexts())
	{
		if (const UGameInstance* GameInstance = World.World()->GetGameInstance())
		{
			if (UNetworkManager* NetworkManager = GameInstance->GetSubsystem<UNetworkManager>())
			{
				if (NetworkManager->GetGameServer() != session)
					continue;
			}
				
			if (UObjectHandler* ObjectFactory = GameInstance->GetSubsystem<UObjectHandler>())
			{
				return ObjectFactory;
			}
		}
	}

	return nullptr;
}


bool Handle_GC_ENTER_GAME(PacketSessionRef& session, Protocol::GC_ENTER_GAME& pkt)
{
	UObjectHandler* Handler = GetObjectHandler(session);
	
	if (Handler)
		Handler->Handle(pkt);
	
	
	return true;
}

bool Handle_GC_SPAWN(PacketSessionRef& session, Protocol::GC_SPAWN& pkt)
{
	UObjectHandler* Handler = GetObjectHandler(session);

	if (Handler)
		Handler->Handle(pkt);
	
	
	return true;
}

bool Handle_GC_DESPAWN(PacketSessionRef& session, Protocol::GC_DESPAWN& pkt)
{
	UObjectHandler* Handler = GetObjectHandler(session);

	if (Handler)
		Handler->Handle(pkt);
	
	return true;
}

bool Handle_GC_REVIVE(PacketSessionRef& session, Protocol::GC_REVIVE& pkt)
{
	UObjectHandler* Handler = GetObjectHandler(session);

	if (Handler)
		Handler->Handle(pkt);
	
	return true;
}

bool Handle_GC_MOVE(PacketSessionRef& session, Protocol::GC_MOVE& pkt)
{
	UObjectHandler* Handler = GetObjectHandler(session);

	if (Handler)
		Handler->Handle(pkt);
	
	return true;
}

bool Handle_GC_ATTACK(PacketSessionRef& session, Protocol::GC_ATTACK& pkt)
{
	UObjectHandler* Handler = GetObjectHandler(session);

	if (Handler)
		Handler->Handle(pkt);
	
	return true;
}

bool Handle_GC_DAMAGE(PacketSessionRef& session, Protocol::GC_DAMAGE& pkt)
{
	UObjectHandler* Handler = GetObjectHandler(session);

	if (Handler)
		Handler->Handle(pkt);
	
	return true;
}
