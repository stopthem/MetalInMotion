// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerSphere.h"
#include "BallBearingGoal.generated.h"

class ABallBearing;
class UParticleSystemComponent;

/**
 * A goal for ball bearings in Metal In Motion
 */
UCLASS()
class ABallBearingGoal : public ATriggerSphere
{
	GENERATED_BODY()

public:
	// Constructor for a goal for ball bearings.
	ABallBearingGoal();

	// Power of the magnetism.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=BallBearing)
	float Magnetism = 7500.0f;

	// Does this goal have a ball bearing resting in its center?
	bool HasBallBearing() const;

	// overlapped ball bearing distance needs to be less than this to has ball bearing
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=BallBearing)
	float MaxDistanceToHasBallBearing = 75.0f;

	// Normal fire vfx, plays when goal doesnt have ball bearing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BallBearing|FireVfx")
	UParticleSystemComponent* NormalFireVfx;

	// Blueish fire vfx, plays when has ball bearing returns true
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BallBearing|FireVfx")
	UParticleSystemComponent* HasBallBearingFireVfx;

protected:
	// Add this ball bearing goal to game mode ball bearing goals
	virtual void BeginPlay() override;

	// Apply magnetism to nearby non player ball bearings and check goals for game end
	virtual void Tick(float DeltaSeconds) override;

	// Hide the collision and sprite components in-game.
	virtual void PostInitializeComponents() override;

	// Add a ball bearing to the list of proximate bearings we're maintaining.
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	// Remove a ball bearing from the list of proximate bearings we're maintaining. 
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

private:
	// A list of proximate ball bearings.
	UPROPERTY()
	TArray<ABallBearing*> BallBearings;

	// is the given actor ball bearing and magnetized
	ABallBearing* IsBallBearingAndMagnetized(AActor* OtherActor) const;

	// If has ball bearing, activate blueish fire if not activate normal.
	void HandleHasBallBearingVfxS(bool hasBallBearing) const;
};
