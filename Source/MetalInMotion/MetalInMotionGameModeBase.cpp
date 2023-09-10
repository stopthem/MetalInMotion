// Copyright Epic Games, Inc. All Rights Reserved.


#include "MetalInMotionGameModeBase.h"

#include "BallBearingGoal.h"
#include "BallBearingHUD.h"
#include "BlueprintGameplayTagLibrary.h"
#include "FCTween.h"
#include "GameplayTagsManager.h"
#include "Algo/AllOf.h"
#include "Algo/AnyOf.h"
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
	if (BallBearingGoals.IsEmpty()) return;

	// Does all ball bearing goals have a ball bearing ?
	const auto allGoalsHasBallBearingGoal = Algo::AllOf(BallBearingGoals, [](const ABallBearingGoal* BallBearingGoal)
	{
		return BallBearingGoal->HasBallBearing();
	});

	// If all ball bearing goals don't have ball bearing and we are counting for game finish 
	if (!allGoalsHasBallBearingGoal && GameFinishedTimer.IsValid())
	{
		GetWorldTimerManager().ClearTimer(GameFinishedTimer);

		return;
	}

	// We already have a timer
	if (GameFinishedTimer.IsValid())
	{
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
