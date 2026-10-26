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

public:
	void SendMovePkt();
protected:
	void 							Move(const FInputActionValue& Value);
	void							Look(const FInputActionValue& Value);
	void 							Attack(const FInputActionValue& Value);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CurrentInput;
	FVector LastSentInput;
	const float MOVEPKT_DELAY = 0.2f;
	float MovePacketTimer = MOVEPKT_DELAY;
	float SnapThreshold = moveSpeed * 0.5f;


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
};
