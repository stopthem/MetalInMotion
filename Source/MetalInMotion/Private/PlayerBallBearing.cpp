// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBallBearing.h"
#include "BallBearing.h"
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

/**
 * @brief
 * Establish the default pawn input bindings for a player ball bearing.
 */
static void InitializeDefaultPawnInputBindings()
{
	if (static bool bBindingsAdded = false; !bBindingsAdded)
	{
		bBindingsAdded = true;

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("BallBearing_MoveLongitudinally", EKeys::W, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("BallBearing_MoveLongitudinally", EKeys::S, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("BallBearing_MoveLongitudinally", EKeys::Up, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("BallBearing_MoveLongitudinally", EKeys::Down, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("BallBearing_MoveLongitudinally", EKeys::Gamepad_LeftY, 1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("BallBearing_MoveLaterally", EKeys::A, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("BallBearing_MoveLaterally", EKeys::D, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("BallBearing_MoveLaterally", EKeys::Left, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("BallBearing_MoveLaterally", EKeys::Right, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("BallBearing_MoveLaterally", EKeys::Gamepad_LeftX, 1.f));

		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("BallBearing_Jump", EKeys::SpaceBar));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("BallBearing_Dash", EKeys::LeftShift));
	}
}

/**
 * @brief
 * Called to bind functionality to input.
 * @param PlayerInputComponent 
 */
void APlayerBallBearing::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InitializeDefaultPawnInputBindings();

	PlayerInputComponent->BindAxis("BallBearing_MoveLongitudinally", this, &APlayerBallBearing::MoveLongitudinally);
	PlayerInputComponent->BindAxis("BallBearing_MoveLaterally", this, &APlayerBallBearing::MoveLaterally);

	PlayerInputComponent->BindAction("BallBearing_Jump", IE_Pressed, this, &APlayerBallBearing::Jump);
	PlayerInputComponent->BindAction("BallBearing_Dash", IE_Pressed, this, &APlayerBallBearing::Dash);
}

/**
 * @brief
 * Have the ball bearing perform a jump.
 */
void APlayerBallBearing::Jump()
{
	// Only jump if there is a contact which usually the ground.
	if (!InContact)return;

	// add impulse upwards for jump
	BallMesh->AddImpulse(FVector(0.0f, 0.0f, JumpForce * 1000.0f));
}

/**
 * @brief
 * Have the ball bearing perform a dash.
 */
void APlayerBallBearing::Dash()
{
	// only dash if not dashing
	if (!bCanDash)return;

	bCanDash = false;

	// change ball bearing state so velocity does not get clamped at max speed
	ChangeBallBearingState(Dashing);

	auto dashDirection = GetInputVector();

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

/**
 * @brief
 * Control the movement of the ball bearing, called every frame.
 * @param deltaSeconds 
 */
void APlayerBallBearing::Tick(float deltaSeconds)
{
	if (CurrentPlayerBallBearingState != Dashing)
	{
		BallMesh->AddForce(GetInputVector() * ControllerForce * BallMesh->GetMass());

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
