// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CreateAccount(FString id, FString password);

	UFUNCTION(BlueprintCallable)
	void Login(FString id, FString password);
};
