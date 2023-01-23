// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBallBearing.h"
#include "BallBearing.h"
#include "FCTween.h"
#include "Kismet/KismetMathLibrary.h"

/**
 * @brief
 * Create a spring-arm and a camera for this ball bearing on object construction.
 */
APlayerBallBearing::APlayerBallBearing()
{
	SetSpringArm();

	SetCamera();

	Magnetized = false;
}

// Create a spring-arm attached to the ball mesh.
void APlayerBallBearing::SetSpringArm()
{
	SpringArm = CreateEditorOnlyDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));

	SpringArm->bDoCollisionTest = false;
	SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
	SpringArm->TargetArmLength = 1000.0f;
	SpringArm->bEnableCameraLag = false;

	SpringArm->SetupAttachment(BallMesh);
}

// Create a camera and attach to the spring-arm.
void APlayerBallBearing::SetCamera()
{
	Camera = CreateEditorOnlyDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	Camera->bUsePawnControlRotation = false;

	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
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

	auto velocity = BallMesh->GetComponentVelocity();

	if (velocity.Size() < 1.0f)return;

	velocity.Normalize();
	velocity *= DashForce * 1000.0f;
	velocity.Z = 0.0f;

	BallMesh->AddImpulse(velocity);

	DashTimerTween = FCTween::Play(0, 1, [&](const float F)
		{
			BrakingRatio = F;
		}, DashTimer, DashEase)
		->SetOnComplete([&]
		{
			bCanDash = true;
		});

	bCanDash = false;
}

/**
 * @brief
 * Control the movement of the ball bearing, called every frame.
 * @param deltaSeconds 
 */
void APlayerBallBearing::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);

	auto velocity = BallMesh->GetComponentVelocity();
	auto const z = velocity.Z;

	velocity.Z = 0;

	if (velocity.Size() > MaximumSpeed * 100.0f)
	{
		velocity.Normalize();
		velocity *= MaximumSpeed * 100.0f;
		velocity.Z = z;

		const auto mergedVelocity = UKismetMathLibrary::VLerp(BallMesh->GetPhysicsLinearVelocity(), velocity, BrakingRatio);

		BallMesh->SetPhysicsLinearVelocity(mergedVelocity);
	}
	else
	{
		BallMesh->AddForce(FVector(InputLongitude, InputLatitude, 0.0f) * ControllerForce * BallMesh->GetMass());
	}
}
