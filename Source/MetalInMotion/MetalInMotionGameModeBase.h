// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Sound/SoundCue.h"
#include "MetalInMotionGameModeBase.generated.h"

class ABallBearingGoal;
/**
 * 
 */
UCLASS()
class METALINMOTION_API AMetalInMotionGameModeBase : public AGameMode
{
	GENERATED_BODY()

public:
	// Construct the game mode, assigning a debugging HUD class.
	AMetalInMotionGameModeBase();

	// The sound cue to play for the background music.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Audio)
	USoundCue* BackgroundMusic = nullptr;

	// The sound cue to play when game finished.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Audio)
	USoundCue* FinishedSound = nullptr;

	// Ball bearing goals calls this when they notified if a ball bearing entered or exited and checks for game end
	void CheckBallBearingGoals();

	// wait this amount when all ball bearings in place and then game end
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=GameEnd)
	float BallBearingsInPlaceBeforeGameEnd = 1.0f;

	// Time after level ended , music played and waiting for level restart
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=GameEnd)
	float TimeBeforeRestart = 5.0f;

	// Add the given ball bearing goal to the list for checking level end
	void AddToBallBearingGoals(ABallBearingGoal* BallBearingGoal);

protected:
	// Play background music beginning of the game
	virtual void BeginPlay() override;

	// Check ball bearing goals for level end
	virtual void Tick(float DeltaSeconds) override;

private:
	// Array of ball bearing goals.
	TArray<TObjectPtr<ABallBearingGoal>> BallBearingGoals;

	// Wait after all ball bearings in place
	FTimerHandle GameFinishedTimer;

	// Handles waiting after game end for restart
	FTimerHandle TimerBeforeRestart;

	// Called when ball bearing goals is completed and game ended
	void GameEnded();

	// The amount of time that the game has been finished.
	float FinishedTime = 0.0f;

	// Has the finished sound been played ?
	bool FinishedSoundPlayed = false;

	// Mainly for clearing all tweens.
	virtual void BeginDestroy() override;
};
