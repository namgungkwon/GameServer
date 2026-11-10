// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObject.h"
#include "InputActionValue.h"
#include "MyPlayer.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API AMyPlayer : public AGameObject
{
	GENERATED_BODY()

public:
	AMyPlayer();

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	void MoveTick(float DeltaTime);
	void UpdateAnimState();
public:
	virtual void SpawnPkt(const Protocol::ObjectData& Pkt) override;
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnPktReceived();

	virtual void RevivePktReceived(const Protocol::GC_REVIVE& pkt) override;
	void RewardPktReceived(const Protocol::GC_REWARD& pkt);
	void LevelUpPktReceived(const Protocol::ObjectData& pkt);
	void ChatPktReceived(FString InSender, FString InMessage);
	UFUNCTION(BlueprintImplementableEvent)
	void ChatReceived();

public:
	UFUNCTION(BlueprintCallable)
	void SendChatPkt(int32 option, FString string);
	void SendMovePkt();
	UFUNCTION(BlueprintCallable)
	void SendRevivePkt(int32 respawnOption);

	UFUNCTION(BlueprintCallable)
	void SendAttackPkt(TArray<AGameObject*> targets, int32 skillId);
	
public:
	void 							Move(const FInputActionValue& Value);
	void							Look(const FInputActionValue& Value);
	void 							Attack(const FInputActionValue& Value);
	UFUNCTION(BlueprintImplementableEvent)
	void AttackInputPressed();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CurrentInput;
	FVector LastSentInput;
	const float CONST_MOVEPKT_DELAY = 0.2f;
	float MovePacketTimer = CONST_MOVEPKT_DELAY;
	float SnapThreshold = moveSpeed * 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CONST_MOVE_DELAY = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float movedelay = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CONST_ATTACK_DELAY = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float attackdelay = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Sender;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Message;


public:
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AttackAction;
};
