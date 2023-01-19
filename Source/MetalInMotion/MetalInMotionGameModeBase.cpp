// Copyright Epic Games, Inc. All Rights Reserved.


#include "MetalInMotionGameModeBase.h"
#include "BallBearingHUD.h"
#include "FCTween.h"

AMetalInMotionGameModeBase::AMetalInMotionGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;

	HUDClass = ABallBearingHUD::StaticClass();
}

void AMetalInMotionGameModeBase::BeginDestroy()
{
	Super::BeginDestroy();

	FCTween::ClearActiveTweens();
}
