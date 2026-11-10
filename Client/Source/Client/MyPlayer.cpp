// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayer.h"

#include "Client.h"
#include "DelayAction.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	//bUseControllerRotationYaw = false;
	//bUseControllerRotationPitch = false;
	//bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
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

	attackdelay -= DeltaTime;
	movedelay -= DeltaTime;

	if (abp_isDead == false)
		MoveTick(DeltaTime);
}

void AMyPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPlayer::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AMyPlayer::Move);
		
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyPlayer::Look);
		
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AMyPlayer::Attack);
	}
}

void AMyPlayer::MoveTick(float DeltaTime)
{
	if (movedelay > 0.f)
		return;
	
	MovePacketTimer -= DeltaTime;

	if (MovePacketTimer <= 0)
	{
		if (CurrentInput.IsNearlyZero())
		{
			MovePacketTimer = CONST_MOVEPKT_DELAY;
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

	FVector InputDirection = FVector::ZeroVector;
	
	if (CurrentInput.IsNearlyZero() == false)
		InputDirection = CurrentInput.GetSafeNormal();

	// 2. 플레이어 Yaw 기준으로 입력 회전
	const float PlayerYaw = GetActorRotation().Yaw;
	const FRotator YawRotation(0.f, PlayerYaw, 0.f);

	const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector Right   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// CurrentInput.Y = 전진/후진, CurrentInput.X = 좌/우라고 가정
	FVector MoveDirection =
		Forward * CurrentInput.X +
		Right   * CurrentInput.Y;

	if (!MoveDirection.IsNearlyZero())
	{
		MoveDirection = MoveDirection.GetSafeNormal();
	}
	
	FVector MoveVelocity = MoveDirection * moveSpeed;

	FVector CorrectionVelocity = Diff * 3.f;
	if (CorrectionVelocity.Size() > 0.5f)
	{
		CorrectionVelocity = CorrectionVelocity.GetSafeNormal() * 0.5f;
	}

	MoveVelocity += CorrectionVelocity;
	
	FVector NextLocation = CurrentLocation + FVector(MoveVelocity.X * DeltaTime, MoveVelocity.Y * DeltaTime, 0.f);

	SetActorLocation(NextLocation, true);
}

void AMyPlayer::SpawnPkt(const Protocol::ObjectData& Pkt)
{
	int a = 500;
	Super::SpawnPkt(Pkt);
	SpawnPktReceived();
}

void AMyPlayer::RevivePktReceived(const Protocol::GC_REVIVE& pkt)
{
	Super::RevivePktReceived(pkt);
}

void AMyPlayer::RewardPktReceived(const Protocol::GC_REWARD& pkt)
{
	Exp += pkt.exp();

	UIUpdate();
}

void AMyPlayer::LevelUpPktReceived(const Protocol::ObjectData& Pkt)
{
	Level = Pkt.summary().level();

	Hp = Pkt.stat().hp();
	MaxHp = Level * 100 + 100;
	Mp = Pkt.stat().mp();
	MaxMp = MaxHp / 2;
	Exp = Pkt.stat().exp();

	int exp = 0;
	for (int i = 1; i <= Level; i++)
	{
		exp += i * 10;
	}
	RequiredExp = exp;

	UIUpdate();
}

void AMyPlayer::ChatPktReceived(FString InSender, FString InMessage)
{
	Sender = InSender;
	Message = InMessage;
	ChatReceived();
}

void AMyPlayer::SendChatPkt(int32 option, FString string)
{
	Protocol::CG_CHAT pkt;
	pkt.set_chat(TCHAR_TO_UTF8(*string));
	Protocol::ChatType type;
	
	switch (option)
	{
	case 0:
			type = Protocol::CHATTYPE_ADJACENT;
		break;
	default:
		type = Protocol::CHATTYPE_WORLD;
		break;
	}

	pkt.set_type(type);
	
	SEND_PACKET(pkt);
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
	MovePacketTimer = CONST_MOVEPKT_DELAY;
}

void AMyPlayer::SendRevivePkt(int32 respawnOption)
{
	Protocol::CG_REVIVE Pkt;
	Pkt.set_objectid(GetObjectId());

	if (respawnOption == 1)
		Pkt.set_option(Protocol::RESPAWNOPTION_SPAWNPOINT);
	else
		Pkt.set_option(Protocol::RESPAWNOPTION_CURPOS);

	SEND_PACKET(Pkt);
}

void AMyPlayer::SendAttackPkt(TArray<AGameObject*> targets, int32 skillId)
{
	Protocol::CG_ATTACK Pkt;
	FVector ActorLocation = GetActorLocation();

	Protocol::ObjectPosData* PosData = Pkt.mutable_posdata();
	PosData->set_objectid(GetObjectId());
	Protocol::Vector3* pos = PosData->mutable_location();
	pos->set_x(ActorLocation.X);
	pos->set_y(ActorLocation.Y);
	pos->set_z(ActorLocation.Z);
	PosData->set_yaw(GetActorRotation().Yaw);

	Pkt.set_skillid(0);

	for (TObjectPtr<AGameObject> target : targets)
	{
		Pkt.add_targets(target->GetObjectId());
	}
	
	SEND_PACKET(Pkt);
}

void AMyPlayer::Move(const FInputActionValue& Value)
{
	CurrentInput = FVector(Value.Get<FVector2D>().X, Value.Get<FVector2D>().Y, 0.0f);

	if (CurrentInput.IsNearlyZero())
	{
		abp_isMoving = false;
	}
	else
	{
		abp_isMoving = true;
	}

	if (LastSentInput != CurrentInput && abp_isDead == false)
	{
		SendMovePkt();
	}
}

void AMyPlayer::Look(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();

	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}

void AMyPlayer::Attack(const FInputActionValue& Value)
{	
	AttackInputPressed();
}
