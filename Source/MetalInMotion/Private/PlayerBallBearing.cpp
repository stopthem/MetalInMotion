// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBallBearing.h"
#include "BallBearing.h"
#include "InputActionValue.h"
#include "Helpers/InterpolationLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

/**
 * @brief
 * Create a spring-arm and a camera for this ball bearing on object construction.
 */
APlayerBallBearing::APlayerBallBearing()
{
	Magnetized = false;
}

// Move the ball bearing with the incoming input value
void APlayerBallBearing::Move(const FInputActionValue& InputActionValue)
{
	InputVector = InputActionValue.Get<FVector>();
	BallMesh->AddForce(InputVector * ControllerForce * BallMesh->GetMass());
}

// Have the ball bearing perform a jump.
void APlayerBallBearing::Jump()
{
	// Only jump if there is a contact which usually the ground.
	if (!InContact)return;

	// add impulse upwards for jump
	BallMesh->AddImpulse(FVector(0.0f, 0.0f, JumpForce * 1000.0f));
}

// Have the ball bearing perform a dash.
void APlayerBallBearing::Dash()
{
	// only dash if not dashing
	if (!bCanDash)return;

	bCanDash = false;

	// change ball bearing state so velocity does not get clamped at max speed
	ChangeBallBearingState(Dashing);

	auto dashDirection = InputVector;

	// if there is no input
	if (dashDirection.Size() == 0)
	{
		auto noInputDashDirection = GetVelocityWithoutUpwards();

		// if ball is barely or not moving in X-Y
		if (noInputDashDirection.Size() <= 1.0f)
		{
			// if there is no input and ball is barely moving, use forward of the actor
			noInputDashDirection = GetActorForwardVector();
		}
		else
		{
			noInputDashDirection.Normalize();
		}

		dashDirection = noInputDashDirection;
	}
	dashDirection.Z = 0;

	// zero out normal velocity because we dont want dash velocity effected by normal velocity
	BallMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
	BallMesh->AddImpulse(dashDirection * DashForce * 1000.0f);

	// first zero out angular velocity and add a angular impulse towards the way we are dashing
	BallMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	BallMesh->AddAngularImpulseInDegrees(FVector(-dashDirection.Y, dashDirection.X, 0.0f) * DashAngularImpulsePower);

	// spawn the dash vfx at actor position
	DashVfxComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DashVfx, GetActorLocation(), FRotator::ZeroRotator);

	// set a timer for dash duration
	GetWorldTimerManager().SetTimer(DashTimer, FTimerDelegate::CreateLambda([&]
	{
		ChangeBallBearingState(InContact ? Grounded : InAir, true);
	}), DashTime, false);

	// set a timer for dash cooldown
	GetWorldTimerManager().SetTimer(DashCooldownTimer, FTimerDelegate::CreateLambda([&]
	{
		bCanDash = true;
	}), DashCoolDown, false);
}

// Control the movement of the ball bearing, called every frame.
void APlayerBallBearing::Tick(float deltaSeconds)
{
	if (CurrentPlayerBallBearingState != Dashing)
	{
		BallMesh->AddForce(InputVector * ControllerForce * BallMesh->GetMass());

		if (GetVelocityWithoutUpwards().Size() > GetMaximumSpeed())
		{
			const auto oldVelocity = BallMesh->GetComponentVelocity();
			auto velocity = oldVelocity;
			velocity.Normalize();
			velocity *= GetMaximumSpeed();
			// doing this because ball bearing loses jumping force if jumped after dashing.
			velocity.Z = oldVelocity.Z;

			const auto mergedVelocity = UInterpolationLibrary::VectorSpringInterpCD(BallMesh->GetComponentVelocity(), velocity, *DashVelocityPtr, GetWorld()->GetDeltaSeconds(), ClampToMaxSpeedInterpSpeed);
			BallMesh->SetPhysicsLinearVelocity(mergedVelocity);
		}
	}

	if (!InContact)ChangeBallBearingState(InAir);
	Super::Tick(deltaSeconds);
}
