// Fill out your copyright notice in the Description page of Project Settings.


#include "BallBearingGoal.h"
#include "Components/BillboardComponent.h"
#include "Components/ShapeComponent.h"

/**
 * @brief Constructor for a goal for ball bearings.
 */
ABallBearingGoal::ABallBearingGoal()
{
	SetActorHiddenInGame(false);
}

/**
 * @brief Hide the collision and sprite components in-game
 */
void ABallBearingGoal::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GetCollisionComponent()->SetHiddenInGame(true);

#if WITH_EDITORONLY_DATA
	GetSpriteComponent()->SetHiddenInGame(true);
#endif
}

/**
 * @brief Add a ball bearing to the list of proximate bearings we're maintaining.
 * @param OtherActor 
 */
void ABallBearingGoal::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	const auto enteringBallBearing = IsBallBearingAndMagnetized(OtherActor);

	if (!enteringBallBearing)
	{
		return;
	}

	BallBearings.AddUnique(enteringBallBearing);
}

/**
	 * @brief Remove a ball bearing from the list of proximate bearings we're maintaining. 
	 * @param OtherActor 
	 */
void ABallBearingGoal::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	const auto exitingBallBearing = IsBallBearingAndMagnetized(OtherActor);

	if (!exitingBallBearing)
	{
		return;
	}

	BallBearings.Remove(exitingBallBearing);
}


ABallBearing* ABallBearingGoal::IsBallBearingAndMagnetized(AActor* OtherActor) const
{
	if (const auto ballBearing = Cast<ABallBearing>(OtherActor); ballBearing && ballBearing->Magnetized)
	{
		return ballBearing;
	}

	return nullptr;
}
