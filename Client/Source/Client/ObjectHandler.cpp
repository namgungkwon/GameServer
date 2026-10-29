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
			SpawnedObjects[id]->Destroy();
			SpawnedObjects.Remove(id);
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
		SpawnedObjects[pkt.objectid()]->Damaged(pkt.damage());
	}
}

/*void UObjectHandler::Spawn(const Protocol::GC_SPAWN_OBJECTS& Pkt)
{
	for (const auto& item : Pkt.spawninfos())
	{
		if (AGameObject* Object = Cast<AGameObject>(Spawn(item.objectid(), item.x(), item.y())))
		{
			//Object->Init(item.objectid());
			SpawnedObjects.Add(item.objectid(), Object);
		}
	}
}

void UObjectHandler::Spawn(const Protocol::GC_SPAWN_PLAYERS& Pkt)
{
	for (const auto& item : Pkt.spawninfos())
	{
		if (AGameObject* Character = Cast<AGameObject>(Spawn(item.objectid(), item.x(), item.y())))
		{
			//Character->Init(item.objectid());
			SpawnedObjects.Add(item.objectid(), Character);
		}
	}
}

void UObjectHandler::Spawn(const Protocol::GC_SPAWN_HERO& Pkt)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	AGameObject* Character = Cast<AGameObject>(PC->GetPawn());
	if (Character == nullptr)
		return;

	//Character->Init(Pkt.spawninfo().objectid());
	MyCharacter = Character;

	//Character->ApplyServerMovement(Pkt.spawninfo().x(), Pkt.spawninfo().y());
}

void UObjectHandler::MovePacketReceived(const Protocol::GC_MOVE& Pkt)
{
	for (const auto& item : Pkt.moveinfo())
	{
		uint64 ObjectId = item.objectid();
		AGameObject* Object = nullptr;
		if (SpawnedObjects.Contains(ObjectId))
		{
			Object = Cast<AGameObject>(*SpawnedObjects.Find(ObjectId));
		}

		if (Object == nullptr)
			return;

		//Object->ApplyServerMovement(item.x(), item.y());
	}
}*/

//void UObjectHandler::AttackPacketReceived(const Protocol::GC_RES_ATTACK& Pkt)
//{
//	uint64 attackerId = Pkt.attackinfo().attackerid();
//
//	TObjectPtr<AGameObject> Attacker = FindObject(attackerId);
//	if (Attacker == nullptr)
//		return;
//
//	//Attacker->ApplyServerAttack(Pkt.attackinfo().skillid(), Pkt.attackinfo().dir());
//}

//void UObjectHandler::DamagePacketReceived(const Protocol::GC_DAMAGE& Pkt)
//{
//	uint64 AttackerId = Pkt.damageinfo().attackerid();
//	TObjectPtr<AGameObject> Attacker = FindObject(AttackerId);
//
//	uint64 TargetId = Pkt.damageinfo().targetid();
//	TObjectPtr<AGameObject> Target = FindObject(TargetId);
//	if (Target == nullptr)
//		return;
//
//	//Target->ApplyServerDamage(this, Attacker, Pkt.damageinfo().damage(), Pkt.damageinfo().isdead());
//}


AActor* UObjectHandler::Spawn(uint64 ObjectId, int32 X, int32 Y)
{
	/*AActor* SpawnedActor = nullptr;

	UWorld* World = GetWorld();
	if (World == nullptr)
		return nullptr;

	const Protocol::ObjectCategory ObjectCategory = Utils::GetObjectCategory(ObjectId);
	const FVector SpawnLocation(X, 0.f, Y);

	if (ObjectCategory == Protocol::OBJECT_CATEGORY_PLAYER)
	{
		if (CharacterClass == nullptr)
			return nullptr;

		if (PlayerObjects.Find(ObjectId) != nullptr)
			return nullptr;

		SpawnedActor = Cast<AActor>(World->SpawnActor(CharacterClass, &SpawnLocation));
	}
	else if (ObjectCategory == Protocol::OBJECT_CATEGORY_MONSTER)
	{
		const uint64 ObjectSubCategory = Utils::GetObjectSubCategory(ObjectId);
		if (MonsterClasses[ObjectSubCategory] == nullptr)
			return nullptr;

		SpawnedActor = Cast<AActor>(World->SpawnActor(MonsterClasses[ObjectSubCategory], &SpawnLocation));
	}

	if (SpawnedActor)
		SpawnedActor->SetActorRotation(FRotator(0, 0, 0));
	return SpawnedActor;*/
	return nullptr;
}

void UObjectHandler::Despawn(uint64 ObjectId)
{
	if (MyCharacter)
	{
		if (MyCharacter->GetObjectId() == ObjectId)
			return;
	}
	
	if (TObjectPtr<AGameObject>* Object = SpawnedObjects.Find(ObjectId))
	{
		SpawnedObjects.Remove(ObjectId);
		(*Object)->Destroy();
	}
}

TObjectPtr<AGameObject> UObjectHandler::FindObject(uint64 ObjectId)
{
	if (SpawnedObjects.Find(ObjectId))
		return SpawnedObjects[ObjectId];

	return nullptr;
}
