#include "GameServerPacketHandler.h"

#include "NetworkManager.h"
//#include "A1/ObjectFactory.h"

//UObjectFactory* GetObjectFactory(PacketSessionRef session)
//{
//	for (auto World : GEngine->GetWorldContexts())
//	{
//		if (const UGameInstance* GameInstance = World.World()->GetGameInstance())
//		{
//			if (UNetworkManager* NetworkManager = GameInstance->GetSubsystem<UNetworkManager>())
//			{
//				if (NetworkManager->GetGameServer() != session)
//					continue;
//			}
//				
//			if (UObjectFactory* ObjectFactory = GameInstance->GetSubsystem<UObjectFactory>())
//			{
//				return ObjectFactory;
//			}
//		}
//	}
//
//	return nullptr;
//}


bool Handle_GC_ENTER_GAME(PacketSessionRef& session, Protocol::GC_ENTER_GAME& pkt)
{
	return true;
}
