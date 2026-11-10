// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectHandler.h"

#include "IDetailTreeNode.h"
#include "Utils.h"
#include "GameObject.h"
#include "IMovieSceneTracksModule.h"
#include "MyPlayer.h"
#include "Kismet/GameplayStatics.h"


bool UObjectHandler::ShouldCreateSubsystem(UObject* Outer) const
{
	if (this->GetClass()->IsInBlueprint() && Super::ShouldCreateSubsystem(Outer))
	{
		return true;
	}
	return false;
}

void UObjectHandler::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UObjectHandler::Deinitialize()
{
	Super::Deinitialize();
}

void UObjectHandler::Destroy(AGameObject* gameObject)
{
	if (SpawnedObjects.Find(gameObject->GetObjectId()) != nullptr)
	{
		SpawnedObjects.Remove(gameObject->GetObjectId());
	}
	if (gameObject->Destroy(true))
	{
		int32 a = 5;
	}
}

void UObjectHandler::Handle(const Protocol::GC_ENTER_GAME& pkt)
{
	uint64 templateId = pkt.object().summary().templateid();
	if (templateId < MyCharacterClass.Num())
	{
		MyCharacter = GetWorld()->SpawnActor<AMyPlayer>(MyCharacterClass[templateId]);

		if (MyCharacter)
		{
			MyCharacter->SpawnPkt(pkt.object());
			APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
			AActor* actor = PC->GetPawn();
			if (actor)
				actor->Destroy();
			PC->UnPossess();
			PC->Possess(MyCharacter);
			PC->SetViewTargetWithBlend(MyCharacter);

			SpawnedObjects.Add(pkt.object().summary().objectid(),MyCharacter);
		}
	}
}

void UObjectHandler::Handle(const Protocol::GC_SPAWN& pkt)
{
	for (const auto& item : pkt.object())
	{
		auto id = item.summary().objectid();

		if (SpawnedObjects.Find(id) != nullptr)
			continue;
		
		auto Type = item.summary().objecttype();
		auto templateId = item.summary().templateid();

		switch(Type)
		{
		case Protocol::OBJECT_CATEGORY_PLAYER:
			{
				if (templateId < CharacterClass.Num())
				{
					if (AGameObject* SpawnedActor = GetWorld()->SpawnActor<AGameObject>(CharacterClass[templateId]))
					{
						SpawnedObjects.Add(id, SpawnedActor);
						SpawnedActor->SpawnPkt(item);
					}
				}
			}
			break;
		case Protocol::OBJECT_CATEGORY_MONSTER:
			{
				if (templateId < MonsterClasses.Num())
				{
					if (AGameObject* SpawnedActor = GetWorld()->SpawnActor<AGameObject>(MonsterClasses[templateId]))
					{
						SpawnedObjects.Add(id, SpawnedActor);
						SpawnedActor->SpawnPkt(item);
					}
				}
			}
			break;
		}
	}
}

void UObjectHandler::Handle(const Protocol::GC_DESPAWN& pkt)
{
	for (uint64 id : pkt.objectid())
	{
		if (SpawnedObjects.Find(id) != nullptr)
		{
			SpawnedObjects[id]->DespawnPkt();
		}	
	}
}

void UObjectHandler::Handle(const Protocol::GC_MOVE& pkt)
{
	for (const Protocol::ObjectPosData& data : pkt.movedata())
	{
		if (SpawnedObjects.Find(data.objectid()) != nullptr)
		{
			SpawnedObjects[data.objectid()]->MovePkt(data);
		}	
	}
}

void UObjectHandler::Handle(const Protocol::GC_ATTACK& pkt)
{
	Protocol::ObjectPosData PosData = pkt.posdata();

	if (SpawnedObjects.Find(PosData.objectid()) != nullptr)
	{
		AGameObject* targetObject = SpawnedObjects[PosData.objectid()];
		targetObject->MovePkt(PosData);
		targetObject->SkillPkt(pkt.skillid());
	}
}

void UObjectHandler::Handle(const Protocol::GC_DAMAGE& pkt)
{
	if (SpawnedObjects.Find(pkt.objectid()) != nullptr)
	{
		SpawnedObjects[pkt.objectid()]->Damaged(pkt.damage(), pkt.isdead());
	}
}

void UObjectHandler::Handle(const Protocol::GC_REVIVE& pkt)
{
	uint64 objectId= pkt.posdata().objectid();
	if (SpawnedObjects.Find(objectId) != nullptr)
	{
		SpawnedObjects[objectId]->RevivePktReceived(pkt);
	}
}

void UObjectHandler::Handle(const Protocol::GC_REWARD& pkt)
{
	if (MyCharacter)
	{
		MyCharacter->RewardPktReceived(pkt);
	}
}

void UObjectHandler::Handle(const Protocol::GC_LEVELUP& pkt)
{
	if (MyCharacter)
	{
		MyCharacter->LevelUpPktReceived(pkt.data());
	}
}

void UObjectHandler::Handle(const Protocol::GC_CHAT& pkt)
{
	FString Sender = FString(pkt.name().c_str());
	FString Chat = FString(pkt.chat().c_str());
	if (MyCharacter)
	{
		MyCharacter->ChatPktReceived(Sender, Chat);
	}	
}

TObjectPtr<AGameObject> UObjectHandler::FindObject(uint64 ObjectId)
{
	if (SpawnedObjects.Find(ObjectId))
		return SpawnedObjects[ObjectId];

	return nullptr;
}
