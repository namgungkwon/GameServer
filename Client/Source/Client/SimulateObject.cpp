// Fill out your copyright notice in the Description page of Project Settings.

#include "SimulateObject.h"

void ASimulateObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SimulateMovement(DeltaTime);
}

void ASimulateObject::MovePkt(const Protocol::ObjectPosData& Pkt)
{
	Super::MovePkt(Pkt);

	SetActorRotation(FRotator(0.f, Pkt.yaw(), 0.f));
}

void ASimulateObject::SimulateMovement(float DeltaTime)
{
	FVector Location = GetActorLocation();
	FVector MoveDir = (serverPos - Location);

	const float DistToDest = MoveDir.Length();
	if (DistToDest <= KINDA_SMALL_NUMBER)
	{
		abp_isMoving = false;
		return;
	}

	abp_isMoving = true;
	const float MoveDist = FMath::Min(moveSpeed * DeltaTime, DistToDest);
	abp_MoveDirection = MoveDir.GetSafeNormal();

	FVector NextLocation = GetActorLocation() + abp_MoveDirection * MoveDist;

	NextLocation.X = FMath::Clamp(NextLocation.X, 0.f, 100000.f);
	NextLocation.Y = FMath::Clamp(NextLocation.Y, 0.f, 100000.f);

	SetActorLocation(NextLocation, true);
}


void ASimulateObject::SkillPkt(uint64 skillId)
{
	AGameObject::SkillPkt(skillId);
	abp_isAttack = true;
}
