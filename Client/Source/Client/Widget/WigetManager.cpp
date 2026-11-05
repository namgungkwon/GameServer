// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetManager.h"

bool UWidgetManager::ShouldCreateSubsystem(UObject* Outer) const
{
	if (this->GetClass()->IsInBlueprint() && Super::ShouldCreateSubsystem(Outer))
	{
		return true;
	}
	return false;
}