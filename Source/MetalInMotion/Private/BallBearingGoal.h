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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=BallBearing)
	float maxDistanceToHasBallBearing = 75.0f;

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
	UPROPERTY(Transient)
	TArray<ABallBearing*> BallBearings;

	ABallBearing* IsBallBearingAndMagnetized(AActor* OtherActor) const;

	UPROPERTY(Transient)
	AMetalInMotionGameModeBase* MetalInMotionGameModeBase;
};
