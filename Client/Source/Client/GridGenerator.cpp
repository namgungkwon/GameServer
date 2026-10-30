// Fill out your copyright notice in the Description page of Project Settings.


#include "GridGenerator.h"

// Sets default values
AGridGenerator::AGridGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGridGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	GenerateCollisionGridFile();
}

bool AGridGenerator::GenerateCollisionGridFile()
{
	const int32 GridCountX = FMath::FloorToInt32((EndX - StartX) / Space) + 1;
	const int32 GridCountY = FMath::FloorToInt32((EndY - StartY) / Space) + 1;

	if (GridCountX <= 0 || GridCountY <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid grid size."));
		return false;
	}

	TArray<uint8> Result;
	Result.Reserve(GridCountX * GridCountY);

	FCollisionQueryParams TraceParams(
		SCENE_QUERY_STAT(GenerateCollisionGrid),
		/*bTraceComplex=*/true
	);

	const FVector TraceStartBase(0.f, 0.f, TraceStartZ);
	const FVector TraceEndBase(0.f, 0.f, TraceEndZ);

	for (int32 Y = 0; Y < GridCountY; ++Y)
	{
		const float WorldY = StartY + Y * Space;

		for (int32 X = 0; X < GridCountX; ++X)
		{
			const float WorldX = StartX + X * Space;

			const FVector Start(WorldX, WorldY, TraceStartZ);
			const FVector End(WorldX, WorldY, TraceEndZ);

			FHitResult Hit;
			const bool bHit = GWorld->LineTraceSingleByChannel(
				Hit,
				Start,
				End,
				ECC_GameTraceChannel1,
				TraceParams
			);

			if (bHit)
			{
				int a = 0;
			}
			Result.Add(bHit ? 1 : 0);
		}
	}

	const bool bSaved = FFileHelper::SaveArrayToFile(
	Result,
	*OutputFilePath
);

	if (!bSaved)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save file: %s"), *OutputFilePath);
		return false;
	}

	UE_LOG(LogTemp, Log,
		TEXT("Saved collision grid. Size=%dx%d, File=%s"),
		GridCountX, GridCountY, *OutputFilePath);

	return true;
}

