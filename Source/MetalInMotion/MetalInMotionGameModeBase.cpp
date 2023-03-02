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

void AMetalInMotionGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckBallBearingGoals();
}

void AMetalInMotionGameModeBase::BeginDestroy()
{
	Super::BeginDestroy();

	FCTween::ClearActiveTweens();
}

void AMetalInMotionGameModeBase::CheckBallBearingGoals()
{
	if (GameFinishedTimer.IsValid())return;

	if (BallBearingGoals.IsEmpty())return;

	auto allGoalsHasBallBearingGoal = true;
	for (const auto ballBearingGoal : BallBearingGoals)
	{
		allGoalsHasBallBearingGoal = !ballBearingGoal->HasBallBearing() ? false : allGoalsHasBallBearingGoal;
	}

	if (!allGoalsHasBallBearingGoal)
	{
		if (GameFinishedTimer.IsValid())GameFinishedTimer.Invalidate();

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
