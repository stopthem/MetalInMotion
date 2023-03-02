// Fill out your copyright notice in the Description page of Project Settings.


#include "BallBearingGoal.h"

#include "PlayerBallBearing.h"
#include "Components/BillboardComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MetalInMotion/MetalInMotionGameModeBase.h"
#include "Particles/ParticleSystemComponent.h"

// Constructor for a goal for ball bearings.
ABallBearingGoal::ABallBearingGoal()
{
	PrimaryActorTick.bCanEverTick = true;

	AActor::SetActorHiddenInGame(false);
}

// Hide the collision and sprite components in-game
void ABallBearingGoal::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GetCollisionComponent()->SetHiddenInGame(true);

	// this causes standalone game crash and i dont have even a slightest clue.
#if WITH_EDITORONLY_DATA
	GetSpriteComponent()->SetHiddenInGame(true);
#endif
}

void ABallBearingGoal::BeginPlay()
{
	Super::BeginPlay();
	Cast<AMetalInMotionGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()))->AddToBallBearingGoals(this);
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
}

// Add a ball bearing to the list of proximate bearings we're maintaining.
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

// Remove a ball bearing from the list of proximate bearings we're maintaining. 
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

// is the given actor ball bearing and magnetized
ABallBearing* ABallBearingGoal::IsBallBearingAndMagnetized(AActor* OtherActor) const
{
	if (const auto ballBearing = Cast<ABallBearing>(OtherActor); ballBearing && ballBearing->Magnetized && typeid(ballBearing) != typeid(APlayerBallBearing))
	{
		return ballBearing;
	}

	return nullptr;
}

// If has ball bearing, activate blueish fire if not activate normal.
void ABallBearingGoal::HandleHasBallBearingVfxS(bool hasBallBearing) const
{
	if (!NormalFireVfx || !HasBallBearingFireVfx)return;

	// only play the particle if not playing
	auto PlayPsIfNotPlaying = [](UParticleSystemComponent& particle)
	{
		if (particle.IsActive())
		{
			return;
		}

		particle.Activate(true);
	};

	const auto deActivatedPs = hasBallBearing ? NormalFireVfx : HasBallBearingFireVfx;
	const auto activatedPs = hasBallBearing ? HasBallBearingFireVfx : NormalFireVfx;
	deActivatedPs->DeactivateImmediate();
	PlayPsIfNotPlaying(*activatedPs);
}

// Does this goal have a ball bearing resting in its center?
bool ABallBearingGoal::HasBallBearing() const
{
	auto hasBallBearing = false;

	const auto ourLocation = GetActorLocation();
	for (const auto ballBearing : BallBearings)
	{
		const auto difference = ourLocation - ballBearing->GetActorLocation();
		const auto distance = difference.Size();

		if (distance < MaxDistanceToHasBallBearing && ballBearing->GetVelocity().Size() <= LessThanVelSizeToHasBallBearing)
		{
			hasBallBearing = true;
		}
	}

	HandleHasBallBearingVfxS(hasBallBearing);
	return hasBallBearing;
}
