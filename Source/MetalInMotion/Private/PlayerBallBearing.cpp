// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBallBearing.h"
#include "BallBearing.h"
#include "InputActionValue.h"
#include "Helpers/InterpolationLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"

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
	BallMesh->AddForce(InputVector * ControllerForce * 100.0f * BallMesh->GetMass());

	if (CurrentPlayerBallBearingState == InAir)
	{
		// Changing incoming input vector to left handed rotation direction
		const auto changedInputVectorToRotationBasedDirection = FVector(-InputVector.Y, InputVector.X, 0);

		// Change ball mesh angular velocity towards wanted angular velocity over time
		BallMesh->SetPhysicsAngularVelocityInDegrees(UKismetMathLibrary::VInterpTo(BallMesh->GetPhysicsAngularVelocityInDegrees(), changedInputVectorToRotationBasedDirection * InAirAngularImpulseTowardsInputPower * 100.0f,
		                                                                           GetWorld()->GetDeltaSeconds(), InAirToAngularVelocitySpeed));
	}
}

// Have the ball bearing perform a jump. 
void APlayerBallBearing::Jump()
{
	// Only jump if there is a contact which usually the ground.
	if (!IsGrounded())return;

	// Zero out upwards velocity so jumping is consistent
	BallMesh->SetPhysicsLinearVelocity(GetVelocityWithoutZ());
	// Add impulse upwards for jump
	BallMesh->AddImpulse(FVector::UpVector * JumpForce * 1000.0f);
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
		auto noInputDashDirection = GetVelocityWithoutZ();

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
	BallMesh->AddAngularImpulseInDegrees(FVector(-dashDirection.Y, dashDirection.X, 0.0f) * DashAngularImpulsePower * 100000.0f);

	// play dash vfx
	DashVfxComponent->ActivateSystem();

	// set a timer for dash duration
	GetWorldTimerManager().SetTimer(DashTimer, FTimerDelegate::CreateLambda([&]
	{
		ChangeBallBearingState(IsGrounded() ? Grounded : InAir, true);
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
		if (GetVelocityWithoutZ().Size() > GetMaximumSpeed())
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

	if (!IsGrounded())ChangeBallBearingState(InAir);
	Super::Tick(deltaSeconds);
}

// Check if the ball bearing is grounded
bool APlayerBallBearing::IsGrounded() const
{
	// set start and end of the trace
	const FVector startTrace = GetActorLocation();
	const FVector endTrace = FVector(startTrace.X, startTrace.Y, startTrace.Z - (GetActorScale().X + SphereTraceGoDistance));

	// ignore this actor when tracing
	TArray<AActor*> ignoredActors;
	ignoredActors.Add(TArray<AActor*>::ElementType(this));

	// trace from center of the actor to down
	FHitResult hit;
	UKismetSystemLibrary::SphereTraceSingle(GetWorld(), startTrace, endTrace, GetActorScale().X * 50.0f * SphereTraceRadiusMultiplier, SphereTraceTypeQuery, true, ignoredActors, EDrawDebugTrace::None, hit, true);
	// DrawDebugLine(GetWorld(), startTrace, endTrace, hit.bBlockingHit ? FColor::Blue : FColor::Red, false, 0.1f, 0, 1.0f);

	// return if trace hit something and its a actor
	return hit.bBlockingHit && IsValid(hit.GetActor());
}
