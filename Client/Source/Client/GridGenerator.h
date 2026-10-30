// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridGenerator.generated.h"

UCLASS()
class CLIENT_API AGridGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	bool GenerateCollisionGridFile();

public:
	UPROPERTY(EditAnywhere)
	FString OutputFilePath;
	UPROPERTY(EditAnywhere)
	float StartX = 50.f;
	UPROPERTY(EditAnywhere)
	float StartY = 50.f;
	UPROPERTY(EditAnywhere)
	float EndX = 50.f;
	UPROPERTY(EditAnywhere)
	float EndY = 50.f;
	UPROPERTY(EditAnywhere)
	float Space = 100.f;
	UPROPERTY(EditAnywhere)
	float TraceStartZ = 10000.f;
	UPROPERTY(EditAnywhere)
	float TraceEndZ = 5.f;
};
