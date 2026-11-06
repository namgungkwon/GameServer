// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Client/NetWork/Protocol.pb.h"
#include "CharacterListWidget.generated.h"

USTRUCT(BlueprintType)
struct FCharacterListWidgetItem
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level;
	
	uint64 ObjectId;
};

/**
 * 
 */
UCLASS()
class CLIENT_API UCharacterListWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetCharacterList(const Protocol::GC_AUTH_TOKEN& pkt);
	void SetCharacterList(const Protocol::GC_CREATE_CHARACTER& pkt);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateCharacterList();

	UFUNCTION(BlueprintCallable)
	void SendEnterGamePkt(int32 listIdx);
	UFUNCTION(BlueprintCallable)
	void SendCreateCharacterPkt(FString name, FString templateId);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCharacterListWidgetItem> CharacterList;	
};
