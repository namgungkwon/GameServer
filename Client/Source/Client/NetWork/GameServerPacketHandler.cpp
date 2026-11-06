#include "GameServerPacketHandler.h"

#include "NetworkManager.h"
#include "Client/Client.h"
#include "Client/ObjectHandler.h"
#include "Client/Widget/CharacterListWidget.h"
#include "Client/Widget/MsgWidget.h"
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
		TObjectPtr<UCharacterListWidget> Widget = CreateWidget<UCharacterListWidget>(WidgetManager->GetGameInstance(), WidgetManager->CharacterListWidgetClass);
		if (Widget)
		{
			WidgetManager->SetCharacterListWidget(Widget);
			Widget->SetCharacterList(pkt);
			Widget->AddToViewport();
		}
	}
	
	return true;
}

bool Handle_GC_CREATE_CHARACTER(PacketSessionRef& session, Protocol::GC_CREATE_CHARACTER& pkt)
{
	UWidgetManager* WidgetManager = GetWidgetManagerG(session);
	if (WidgetManager == nullptr)
		return true;

	UMsgWidget* Widget = CreateWidget<UMsgWidget>(WidgetManager->GetGameInstance(), WidgetManager->MsgWidgetClass);
	if (Widget == nullptr)
		return true;
	
	switch (pkt.result())
	{
	case Protocol::CREATECHARACTER_LENGTH:
		Widget->SetMessage(TEXT("캐릭터 이름 길이 오류"));
		break;
	case Protocol::CREATEACCOUNT_DUPLICATION:
		Widget->SetMessage(TEXT("중복된 이름"));
		break;
	case Protocol::SUCCESS:
		Widget->SetMessage(TEXT("캐릭터 생성 성공!"));
		break;
	}
	
	if (pkt.result() == Protocol::SUCCESS)
	{
		TObjectPtr<UCharacterListWidget> ListWidget = WidgetManager->GetCharacterListWidget();
		if (IsValid(ListWidget) == false)
		{
			ListWidget = nullptr;
			ListWidget = CreateWidget<UCharacterListWidget>(WidgetManager->GetGameInstance(), WidgetManager->CharacterListWidgetClass);
		}

		if (ListWidget != nullptr)
		{
			ListWidget->SetCharacterList(pkt);
			ListWidget->AddToViewport();
		}
	}
	Widget->AddToViewport();
		
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
