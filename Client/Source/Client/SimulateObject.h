// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObject.h"
#include "SimulateObject.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API ASimulateObject : public AGameObject
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;

public:
	virtual void MovePkt(const Protocol::ObjectPosData& Pkt) override;

public:
	void SimulateMovement(float DeltaTime);
};
