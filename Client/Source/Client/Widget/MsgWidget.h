// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MsgWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API UMsgWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetMessage(const FString& Message);

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> MessageText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> CloseButton;
};