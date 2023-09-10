// Fill out your copyright notice in the Description page of Project Settings.


#include "BallBearingGoal.h"

#include "PlayerBallBearing.h"
#include "Algo/AnyOf.h"
#include "Components/BillboardComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "MetalInMotion/MetalInMotionGameModeBase.h"
#include "Particles/ParticleSystemComponent.h"

ABallBearingGoal::ABallBearingGoal()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABallBearingGoal::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GetSpriteComponent()->SetHiddenInGame(true);

	GetCollisionComponent()->SetHiddenInGame(true);
}

void ABallBearingGoal::BeginPlay()
{
	Super::BeginPlay();

	Cast<AMetalInMotionGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()))->AddToBallBearingGoals(this);

	HandleHasBallBearingVfX();
}

void ABallBearingGoal::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	HandleBallMagnetism();

	CheckHasBallBearing();
}

void ABallBearingGoal::HandleBallMagnetism()
{
	const auto ourLocation = GetActorLocation();
	const auto sphereRadius = Cast<USphereComponent>(GetCollisionComponent())->GetScaledSphereRadius();
	auto magnetism = Magnetism;

	// If we're cheating then give our goals extra magnetism.
	const auto extraForce = IConsoleManager::Get().FindConsoleVariable(TEXT("MetalInMotion.ExtraMagnetism"));
	if (extraForce && extraForce->GetInt() != 0)
	{
		magnetism *= 4.0f;
	}

	// Calculate force on every ball bearing in our range and apply it
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

void ABallBearingGoal::CheckHasBallBearing()
{
	const auto ourLocation = GetActorLocation();

	// Does any of the ball bearings in our range meets conditions for has ball bearing ?
	const auto bTempHasBallBearing = Algo::AnyOf(BallBearings, [&,ourLocation](const ABallBearing* ballBearing)
	{
		const auto difference = ourLocation - ballBearing->GetActorLocation();
		const auto distance = difference.Size();

		if (distance <= MaxDistanceToHasBallBearing && ballBearing->GetVelocity().Size() <= LessThanVelSizeToHasBallBearing)
		{
			return true;
		}

		return false;
	});

	// If our ball bearing having status is changed
	if (bHasBallBearing != bTempHasBallBearing)
	{
		if (bTempHasBallBearing) // We didn't have before, now we have it
		{
			// We have potential, don't do anything
			// We need this because we don't set has ball bearing true without waiting
			if (bPotentialHasBallBearing)
			{
				return;
			}

			bPotentialHasBallBearing = true;

			GetWorldTimerManager().SetTimer(ConfirmHasBallBearingTimerHandle, FTimerDelegate::CreateLambda([=]
			{
				// We have confirmed that we have a ball bearing
				bHasBallBearing = true;
				HandleHasBallBearingVfX();
			}), TimeToWaitForConfirmHasBallBearing, false);
		}
		else // We had it before, now we don't
		{
			GetWorldTimerManager().ClearTimer(ConfirmHasBallBearingTimerHandle);

			bHasBallBearing = false;

			bPotentialHasBallBearing = false;

			HandleHasBallBearingVfX();
		}
	}
}

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
	if (const auto ballBearing = Cast<ABallBearing>(OtherActor); ballBearing && ballBearing->Magnetized && !ballBearing->IsPlayerControlled())
	{
		return ballBearing;
	}

	return nullptr;
}

void ABallBearingGoal::HandleHasBallBearingVfX() const
{
	const auto deActivatedPs = bHasBallBearing ? NormalFireVfx : HasBallBearingFireVfx;
	deActivatedPs->Deactivate();

	const auto activatedPs = bHasBallBearing ? HasBallBearingFireVfx : NormalFireVfx;
	activatedPs->Activate(true);
}
