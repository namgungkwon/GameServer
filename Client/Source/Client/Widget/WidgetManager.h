// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WidgetManager.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class CLIENT_API UWidgetManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<UUserWidget> MsgWidgetClass;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<UUserWidget> CharacterListWidget;
};
