// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BallBearing.h"
#include "Engine/TriggerSphere.h"
#include "MetalInMotion/MetalInMotionGameModeBase.h"
#include "BallBearingGoal.generated.h"

/**
 * A goal for ball bearings in Metal In Motion
 */
UCLASS()
class ABallBearingGoal : public ATriggerSphere
{
	GENERATED_BODY()

public:
	/**
	 * @brief Constructor for a goal for ball bearings.
	 */
	ABallBearingGoal();

	/**
	 * @brief Power of the magnetism.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=BallBearing)
	float Magnetism = 7500.0f;

	/**
	* @brief Does this goal have a ball bearing resting in its center?
	* @return 
	*/
	bool HasBallBearing() const;

	// overlapped ball bearing distance needs to be less than this to has ball bearing
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=BallBearing)
	float maxDistanceToHasBallBearing = 75.0f;

	// The color of the fire when has ball bearing is true
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BallBearing|FireVfx")
	FLinearColor HasBallBearingFireColor;

	// the material index of fire
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BallBearing|FireVfx")
	int32 FireVfxColorChangeMatIndex;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	/**
	 * @brief Hide the collision and sprite components in-game.
	 */
	virtual void PostInitializeComponents() override;

	/**
	 * @brief Add a ball bearing to the list of proximate bearings we're maintaining.
	 * @param OtherActor 
	 */
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	/**
	 * @brief Remove a ball bearing from the list of proximate bearings we're maintaining. 
	 * @param OtherActor 
	 */
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

private:
	/**
	 * @brief A list of proximate ball bearings.
	 */
	UPROPERTY()
	TArray<ABallBearing*> BallBearings;

	// is the given actor ball bearing and magnetized
	ABallBearing* IsBallBearingAndMagnetized(AActor* OtherActor) const;

	// the game mode
	UPROPERTY()
	AMetalInMotionGameModeBase* MetalInMotionGameModeBase;

	// the fire vfx
	UPROPERTY()
	UParticleSystemComponent* FireVfxComponent;

	// the fire material in fire vfx
	UPROPERTY()
	UMaterialInterface* FireVfxFireMat;

	// the starting fire color
	FLinearColor* FireVfxFireStartColor;
};
