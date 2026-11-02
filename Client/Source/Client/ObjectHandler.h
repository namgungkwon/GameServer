// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Network/Protocol.pb.h"
#include "GameObject.h"
#include "ObjectHandler.generated.h"

class AMyPlayer;
class AGameObject;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class CLIENT_API UObjectHandler : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	UFUNCTION(BlueprintCallable)
	void Destroy(AGameObject* gameObject);
public:
	void Handle(const Protocol::GC_ENTER_GAME& pkt);
	void Handle(const Protocol::GC_SPAWN& pkt);
	void Handle(const Protocol::GC_DESPAWN& pkt);
	void Handle(const Protocol::GC_MOVE& pkt);
	void Handle(const Protocol::GC_ATTACK& pkt);
	void Handle(const Protocol::GC_DAMAGE& pkt);
	void Handle(const Protocol::GC_REVIVE& pkt);
	void Handle(const Protocol::GC_REWARD& pkt);
	void Handle(const Protocol::GC_LEVELUP& pkt);
	
	
public:
	void Despawn(uint64 ObjectId);
	TObjectPtr<AGameObject> FindObject(uint64 ObjectId);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AGameObject>>				MonsterClasses;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AGameObject>>				CharacterClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AGameObject>>				MyCharacterClass;
	
public:
	UPROPERTY()
	TMap<uint64, TObjectPtr<AGameObject>>	SpawnedObjects;
	UPROPERTY()
	TObjectPtr<AMyPlayer>				MyCharacter;
};