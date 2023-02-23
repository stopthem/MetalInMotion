// Fill out your copyright notice in the Description page of Project Settings.


#include "BallBearingGoal.h"
#include "Components/BillboardComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

/**
 * @brief Constructor for a goal for ball bearings.
 */
ABallBearingGoal::ABallBearingGoal()
{
	PrimaryActorTick.bCanEverTick = true;

	AActor::SetActorHiddenInGame(false);
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

	MetalInMotionGameModeBase = Cast<AMetalInMotionGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
}

void ABallBearingGoal::BeginPlay()
{
	Super::BeginPlay();

	MetalInMotionGameModeBase->AddToBallBearingGoals(this);
}


void ABallBearingGoal::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const auto ourLocation = GetActorLocation();
	const auto sphereRadius = Cast<USphereComponent>(GetCollisionComponent())->GetScaledSphereRadius();
	auto magnetism = Magnetism;

	// If we're cheating then give our goals extra magnetism.

	const auto extraForce = IConsoleManager::Get().FindConsoleVariable(TEXT("MetalInMotion.ExtraMagnetism"));
	if (extraForce && extraForce->GetInt() != 0)
	{
		magnetism *= 4.0f;
	}

	for (const ABallBearing* ballBearing : BallBearings)
	{
		const auto difference = ourLocation - ballBearing->GetActorLocation();
		const auto distance = difference.Size();

		auto direction = difference;
		direction.Normalize();

		const auto ratio = distance / sphereRadius;
		const auto force = (1.0f - ratio) * magnetism * direction;

		ballBearing->BallMesh->AddForce(force);
	}

	if (!HasBallBearing())return;
	MetalInMotionGameModeBase->CheckBallBearingGoals();
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

bool ABallBearingGoal::HasBallBearing() const
{
	const auto ourLocation = GetActorLocation();
	for (const auto ballBearing : BallBearings)
	{
		const auto difference = ourLocation - ballBearing->GetActorLocation();
		const auto distance = difference.Size();

		if (distance < maxDistanceToHasBallBearing)
		{
			return true;
		}
	}
	return false;
}
