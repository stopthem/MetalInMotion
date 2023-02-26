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
	/**
	 * @brief Construct the game mode, assigning a debugging HUD class.
	 */
	AMetalInMotionGameModeBase();

	/**
	 * @brief The sound cue to play for the background music.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Audio)
	USoundCue* BackgroundMusic = nullptr;

	/**
	 * @brief The sound cue to play when game finished.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Audio)
	USoundCue* FinishedSound = nullptr;

	/**
	 * @brief Ball bearing goals calls this when they notified if a ball bearing entered or exited and checks for game end
	 */
	void CheckBallBearingGoals();

	/**
	 * @brief wait this amount when all ball bearings in place and then game end
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=GameEnd)
	float BallBearingsInPlaceBeforeGameEnd = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=GameEnd)
	float TimeBeforeRestart = 5.0f;

	void AddToBallBearingGoals(ABallBearingGoal* BallBearingGoal);

protected:
	/**
	 * @brief Play background music beginning of the game
	 */
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

private:
	/**
	* @brief Array of ball bearing goals.
	*/
	UPROPERTY(Transient)
	TArray<ABallBearingGoal*> BallBearingGoals;

	/**
	 * @brief Wait after all ball bearings in place
	 */
	FTimerHandle GameFinishedTimer;
	/**
	 * @brief Handles waiting after game end for restart
	 */
	FTimerHandle TimerBeforeRestart;

	/**
	 * @brief called when ball bearing goals is completed and game ended
	 */
	void GameEnded();

	/**
	 * @brief The amount of time that the game has been finished.
	 */
	float FinishedTime = 0.0f;

	/**
	 * @brief Has the finished sound been played ?
	 */
	bool FinishedSoundPlayed = false;

	/**
	 * @brief mainly for clearing all tweens.
	 */
	virtual void BeginDestroy() override;
};
