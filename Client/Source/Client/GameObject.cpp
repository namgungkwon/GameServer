// Fill out your copyright notice in the Description page of Project Settings.


#include "GameObject.h"

// Sets default values
AGameObject::AGameObject()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGameObject::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGameObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AGameObject::SpawnPkt(const Protocol::ObjectData& Pkt)
{
	ObjectId = Pkt.summary().objectid();
	TemplateId = Pkt.summary().templateid();
	Name = FString(Pkt.summary().name().c_str());
	Level = Pkt.summary().level();
	Type = Pkt.summary().objecttype();

	serverPos = FVector(Pkt.pos().location().x(), Pkt.pos().location().y(), Pkt.pos().location().z());
	Yaw = Pkt.pos().yaw();

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

	SetActorLocation(serverPos);
}

void AGameObject::MovePkt(const Protocol::ObjectPosData& Pkt)
{
	serverPos = FVector(Pkt.location().x(), Pkt.location().y(), Pkt.location().z() + 90.f);
}

void AGameObject::Damaged(uint64 damage)
{
	if (damage > Hp)
		Hp = 0;
	else
	{
		Hp -= damage;
	}
	HpChanged();
}



