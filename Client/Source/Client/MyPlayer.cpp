// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayer.h"

#include "Client.h"
#include "DelayAction.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

AMyPlayer::AMyPlayer()
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character
	//temp
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->SetWorldRotation(FRotator(0.f, -90.f, 0.f));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

}

void AMyPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	CurrentInput= FVector::ZeroVector;
}

void AMyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MovePacketTimer -= DeltaTime;

	if (MovePacketTimer <= 0)
	{
		if (CurrentInput.IsNearlyZero())
		{
			MovePacketTimer = MOVEPKT_DELAY;
		}
		else
		{
			SendMovePkt();
		}
	}

	FVector CurrentLocation = GetActorLocation();
	
	FVector Diff = serverPos - CurrentLocation;
	FVector temp = FVector(Diff.X, Diff.Y, 0.f);
	float Distance = temp.Size();
	
	if (Distance > SnapThreshold)
	{
		SetActorLocation(serverPos, true);

		return;
	}

	// 작게 어긋났으면 부드럽게 보정
	/*SetActorLocation(FMath::VInterpTo(
		CurrentLocation,
		DesiredLocation,
		GetWorld()->GetDeltaSeconds(),
		MoveSpeedY * interptorate
	));*/

	FVector InputDirection = FVector::ZeroVector;
	
	if (CurrentInput.IsNearlyZero() == false)
		InputDirection = CurrentInput.GetSafeNormal();

	FVector MoveVelocity = InputDirection * FVector(moveSpeed, moveSpeed, 0.f);

	FVector CorrectionVelocity = Diff * 3.f;
	if (CorrectionVelocity.Size() > 0.5f)
	{
		CorrectionVelocity = CorrectionVelocity.GetSafeNormal() * 0.5f;
	}

	MoveVelocity += CorrectionVelocity;
	
	FVector NextLocation = CurrentLocation + FVector(MoveVelocity.X * DeltaTime, MoveVelocity.Y * DeltaTime, 0.f);

	SetActorLocation(NextLocation);
}

void AMyPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPlayer::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AMyPlayer::Move);
	}
}

void AMyPlayer::SpawnPkt(const Protocol::ObjectData& Pkt)
{
	int a = 500;
	Super::SpawnPkt(Pkt);
	SpawnPktReceived();
}


void AMyPlayer::SendMovePkt()
{
	Protocol::CG_MOVE Pkt;
	
	Protocol::ObjectMoveData* moveData = Pkt.mutable_movedata();
	moveData->set_objectid(GetObjectId());

	Protocol::Vector3* Location = moveData->mutable_location();
	FVector ActorLocation = GetActorLocation();
	Location->set_x(ActorLocation.X);
	Location->set_y(ActorLocation.Y);
	Location->set_z(ActorLocation.Z);

	Protocol::Vector3* vec = moveData->mutable_velocity();
	vec->set_x(CurrentInput.X);
	vec->set_y(CurrentInput.Y);
	vec->set_z(CurrentInput.Z);
	
	moveData->set_yaw(GetActorRotation().Yaw);

	if (CurrentInput.IsNearlyZero())
	{
		int a = 0;
	}
	
	SEND_PACKET(Pkt);
	LastSentInput = CurrentInput;
	MovePacketTimer = MOVEPKT_DELAY;
}

void AMyPlayer::Move(const FInputActionValue& Value)
{
	CurrentInput = FVector(Value.Get<FVector2D>().X, Value.Get<FVector2D>().Y, 0.0f);

	if (CurrentInput.IsNearlyZero())
	{
		int a = 0;
	}

	if (LastSentInput != CurrentInput)
	{
		SendMovePkt();
	}
}

void AMyPlayer::Attack(const FInputActionValue& Value)
{
	
}
