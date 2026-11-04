#include "LoginServerPacketHandler.h"

#include "NetworkManager.h"
#include "absl/random/internal/nanobenchmark.h"
#include "Client/Client.h"
#include "Client/ObjectHandler.h"
#include "Kismet/GameplayStatics.h"

UNetworkManager* GetNetworkManager(PacketSessionRef session)
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

bool Handle_LC_CREATE_ACCOUNT(PacketSessionRef& session, Protocol::LC_CREATE_ACCOUNT& pkt)
{
	Protocol::Result result = pkt.result();

	{
		// TODO 알림창
	}
	
	
	return true;
}

bool Handle_LC_LOGIN(PacketSessionRef& session, Protocol::LC_LOGIN& pkt)
{
	Protocol::Result result = pkt.result();

	if (result != Protocol::SUCCESS)
	{
		// TODO 알림창
		return true;
	}

	UNetworkManager* NetworkManager = GetNetworkManager(session);
	
	if (NetworkManager)
		NetworkManager->token = FString(pkt.token().c_str());


	UGameplayStatics::OpenLevel(GWorld, "DefaultMap", true);
	
	return true;
}
