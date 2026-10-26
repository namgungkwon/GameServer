// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Network/Protocol.pb.h"
#include "GameObject.generated.h"

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
	virtual void SpawnPkt(const Protocol::ObjectData& Pkt);
	virtual void MovePkt(const Protocol::ObjectPosData& Pkt);
	
	virtual void ApplyServerPosition(float x, float y) {};
	virtual void SetHP(float currentHp, float currentMp) {};

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool abp_isMoving = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector abp_MoveDirection;
	
public:
	uint64 ObjectId = 0;
	uint64 TemplateId = 0;
	Protocol::ObjectCategory Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Hp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Mp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxMp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector serverPos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Yaw = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Exp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RequiredExp = 0;

	float moveSpeed = 300.f;
};
