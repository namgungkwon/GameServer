#include "GameServerPacketHandler.h"

#include "NetworkManager.h"
#include "Client/Client.h"
#include "Client/ObjectHandler.h"
#include "Client/Widget/CharacterListWidget.h"
#include "Client/Widget/WidgetManager.h"

UObjectHandler* GetObjectHandlerG(PacketSessionRef session)
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

UNetworkManager* GetNetworkManagerG(PacketSessionRef session)
{
	for (auto World : GEngine->GetWorldContexts())
	{
		if (const UGameInstance* GameInstance = World.World()->GetGameInstance())
		{
			if (UNetworkManager* NetworkManager = GameInstance->GetSubsystem<UNetworkManager>())
			{
				if (NetworkManager->GetGameServer() != session)
					continue;

				return NetworkManager;
			}
		}
	}
	return nullptr;
}

UWidgetManager* GetWidgetManagerG(PacketSessionRef session)
{
	for (auto World : GEngine->GetWorldContexts())
	{
		if (const UGameInstance* GameInstance = World.World()->GetGameInstance())
		{
			if (UNetworkManager* NetworkManager = GameInstance->GetSubsystem<UNetworkManager>())
			{
				if (NetworkManager->GetGameServer() != session)
					continue;

				if (UWidgetManager* WidgetManager = GameInstance->GetSubsystem<UWidgetManager>())
				{
					return WidgetManager;
				}
			}
		}
	}
	return nullptr;
}

bool Handle_GC_AUTH_TOKEN(PacketSessionRef& session, Protocol::GC_AUTH_TOKEN& pkt)
{
	if (pkt.result() != Protocol::SUCCESS)
	{
		UNetworkManager* NetworkManager = GetNetworkManagerG(session);
		if (NetworkManager)
		{
			NetworkManager->OpenLoginLevel();
		}
		return true;
	}

	UWidgetManager* WidgetManager = GetWidgetManagerG(session);
	if (WidgetManager)
	{
		UCharacterListWidget* Widget = CreateWidget<UCharacterListWidget>(WidgetManager->GetGameInstance(), WidgetManager->CharacterListWidget);
		if (Widget)
		{
			Widget->SetCharacterList(pkt);
		}
		Widget->AddToViewport();
	}
	
	return true;
}

bool Handle_GC_ENTER_GAME(PacketSessionRef& session, Protocol::GC_ENTER_GAME& pkt)
{
	UObjectHandler* Handler = GetObjectHandlerG(session);
	
	if (Handler)
		Handler->Handle(pkt);
	
	
	return true;
}

bool Handle_GC_SPAWN(PacketSessionRef& session, Protocol::GC_SPAWN& pkt)
{
	UObjectHandler* Handler = GetObjectHandlerG(session);

	if (Handler)
		Handler->Handle(pkt);
	
	
	return true;
}

bool Handle_GC_DESPAWN(PacketSessionRef& session, Protocol::GC_DESPAWN& pkt)
{
	UObjectHandler* Handler = GetObjectHandlerG(session);

	if (Handler)
		Handler->Handle(pkt);
	
	return true;
}

bool Handle_GC_REVIVE(PacketSessionRef& session, Protocol::GC_REVIVE& pkt)
{
	UObjectHandler* Handler = GetObjectHandlerG(session);

	if (Handler)
		Handler->Handle(pkt);
	
	return true;
}

bool Handle_GC_MOVE(PacketSessionRef& session, Protocol::GC_MOVE& pkt)
{
	UObjectHandler* Handler = GetObjectHandlerG(session);

	if (Handler)
		Handler->Handle(pkt);
	
	return true;
}

bool Handle_GC_ATTACK(PacketSessionRef& session, Protocol::GC_ATTACK& pkt)
{
	UObjectHandler* Handler = GetObjectHandlerG(session);

	if (Handler)
		Handler->Handle(pkt);
	
	return true;
}

bool Handle_GC_DAMAGE(PacketSessionRef& session, Protocol::GC_DAMAGE& pkt)
{
	UObjectHandler* Handler = GetObjectHandlerG(session);

	if (Handler)
		Handler->Handle(pkt);
	
	return true;
}

bool Handle_GC_REWARD(PacketSessionRef& session, Protocol::GC_REWARD& pkt)
{
	UObjectHandler* Handler = GetObjectHandlerG(session);

	if (Handler)
		Handler->Handle(pkt);
	
	return true;
}

bool Handle_GC_LEVELUP(PacketSessionRef& session, Protocol::GC_LEVELUP& pkt)
{
	UObjectHandler* Handler = GetObjectHandlerG(session);

	if (Handler)
		Handler->Handle(pkt);
	
	return true;
}

bool Handle_GC_CHAT(PacketSessionRef& session, Protocol::GC_CHAT& pkt)
{
	UObjectHandler* Handler = GetObjectHandlerG(session);

	if (Handler)
		Handler->Handle(pkt);
	
	return true;
}
