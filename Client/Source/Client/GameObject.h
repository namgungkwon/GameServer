// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameObject.generated.h"

struct FObjectInfo
{
	uint64 objectId = 0;
	FString name = FString();
	int32 Hp = 0;
	int32 Mp = 0;
	FVector pos = FVector();
};

UCLASS()
class CLIENT_API AGameObject : public ACharacter
{
	GENERATED_BODY()

public:
	AGameObject();
	virtual void Tick(float DeltaTime) override;
	uint64 GetObjectId() { return ObjectId; }

protected:
	virtual void BeginPlay() override;

public:
	virtual void Init(const FObjectInfo& InObjectId) {};
	
	virtual void ApplyServerPosition(float x, float y) {};
	virtual void SetHP(float currentHp, float currentMp) {};
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	uint64 ObjectId = 0;
	FString Name;
	int32 Hp;
	int32 Mp;
	FVector serverPos;
};
