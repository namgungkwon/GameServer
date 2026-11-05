#include "LoginServerPacketHandler.h"

#include "NetworkManager.h"
#include "absl/random/internal/nanobenchmark.h"
#include "Blueprint/UserWidget.h"
#include "Client/Client.h"
#include "Client/ObjectHandler.h"
#include "Client/Widget/MsgWidget.h"
#include "Client/Widget/WidgetManager.h"
#include "Kismet/GameplayStatics.h"

UNetworkManager* GetNetworkManagerL(PacketSessionRef session)
{
	for (auto World : GEngine->GetWorldContexts())
	{
		if (const UGameInstance* GameInstance = World.World()->GetGameInstance())
		{
			if (UNetworkManager* NetworkManager = GameInstance->GetSubsystem<UNetworkManager>())
			{
				if (NetworkManager->GetLoginServer() != session)
					continue;

				return NetworkManager;
			}
		}
	}
	return nullptr;
}

UWidgetManager* GetWidgetManagerL(PacketSessionRef session)
{
	for (auto World : GEngine->GetWorldContexts())
	{
		if (const UGameInstance* GameInstance = World.World()->GetGameInstance())
		{
			if (UNetworkManager* NetworkManager = GameInstance->GetSubsystem<UNetworkManager>())
			{
				if (NetworkManager->GetLoginServer() != session)
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

bool Handle_LC_CREATE_ACCOUNT(PacketSessionRef& session, Protocol::LC_CREATE_ACCOUNT& pkt)
{
	Protocol::Result result = pkt.result();

	UWidgetManager* WidgetManager = GetWidgetManagerL(session);

	if (WidgetManager == nullptr)
		return true;

	UMsgWidget* Widget = CreateWidget<UMsgWidget>(WidgetManager->GetGameInstance(), WidgetManager->MsgWidgetClass);

	if (Widget == nullptr)
		return true;

	switch(result)
	{
		case Protocol::Result::SUCCESS:
			Widget->SetMessage(TEXT("계정 생성!"));
			break;
	case Protocol::CREATEACCOUNT_LENGTH:
		Widget->SetMessage(TEXT("아이디와 비밀번호의 길이가 적당하지 않음"));
		break;
	case Protocol::CREATEACCOUNT_DUPLICATION:
		Widget->SetMessage(TEXT("중복된 ID"));
		break;
	}

	Widget->AddToViewport();

	return true;
}

bool Handle_LC_LOGIN(PacketSessionRef& session, Protocol::LC_LOGIN& pkt)
{
	Protocol::Result result = pkt.result();

	if (result != Protocol::Result::SUCCESS)
	{
		UWidgetManager* WidgetManager = GetWidgetManagerL(session);

		if (WidgetManager == nullptr)
			return true;

		UMsgWidget* Widget = CreateWidget<UMsgWidget>(WidgetManager->GetGameInstance(), WidgetManager->MsgWidgetClass);

		if (Widget == nullptr)
			return true;
		
		switch(result)
		{
		case Protocol::LOGIN_INVALID_ACCOUNT:
			Widget->SetMessage(TEXT("유효하지 않은 계정"));
			break;
		}

		Widget->AddToViewport();
		return true;
	}
	
	UNetworkManager* NetworkManager = GetNetworkManagerL(session);
	if (NetworkManager)
	{
		NetworkManager->token = FString(pkt.token().c_str());
		NetworkManager->OpenGameLevel();
	}

	return true;
}
