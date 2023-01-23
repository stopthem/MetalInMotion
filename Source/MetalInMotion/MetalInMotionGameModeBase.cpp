// Copyright Epic Games, Inc. All Rights Reserved.


#include "MetalInMotionGameModeBase.h"

#include "BallBearingGoal.h"
#include "BallBearingHUD.h"
#include "FCTween.h"
#include "Kismet/GameplayStatics.h"

AMetalInMotionGameModeBase::AMetalInMotionGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;

	HUDClass = ABallBearingHUD::StaticClass();
}

void AMetalInMotionGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::PlaySound2D(AActor::GetWorld(), BackgroundMusic);
}

void AMetalInMotionGameModeBase::BeginDestroy()
{
	Super::BeginDestroy();

	FCTween::ClearActiveTweens();
}

void AMetalInMotionGameModeBase::CheckBallBearingGoals()
{
	// HasBallBearingGoalCount = hasBallBearing ? HasBallBearingGoalCount + 1 : HasBallBearingGoalCount - 1;

	if (BallBearingGoals.IsEmpty())return;

	if (BallBearingGoals.FindByPredicate([](const ABallBearingGoal* BallBearingGoal)
		{
			return !BallBearingGoal->HasBallBearing();
		})
	)
	{
		GameFinishedTimer.Invalidate();
		return;
	}

	GetWorldTimerManager().SetTimer(GameFinishedTimer, this, &AMetalInMotionGameModeBase::GameEnded, BallBearingsInPlaceBeforeGameEnd);
}

void AMetalInMotionGameModeBase::GameEnded()
{
	UGameplayStatics::PlaySound2D(AActor::GetWorld(), FinishedSound);

	GetWorldTimerManager().SetTimer(TimerBeforeRestart, this, &Super::RestartGame, TimeBeforeRestart);
}

void AMetalInMotionGameModeBase::AddToBallBearingGoals(ABallBearingGoal* BallBearingGoal)
{
	BallBearingGoals.Add(BallBearingGoal);
}
