// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BallBearing.h"
#include "PlayerBallBearing.generated.h"

UENUM()
enum EPlayerBallBearingState
{
	InAir,
	Dashing,
	Grounded
};

/**
 * The ball bearing that player controls
 */
UCLASS()
class METALINMOTION_API APlayerBallBearing : public ABallBearing
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties.
	APlayerBallBearing();

	// How much force to use to push the ball bearing around.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=BallBearing)
	float ControllerForce = 250.0f;

	// The maximum speed in meters per second
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=BallBearing)
	float MaximumSpeed = 4.0f;

	// The speed of damping velocity back to at least maximum speed's speed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=BallBearing)
	float ClampToMaxSpeedInterpSpeed = 5.f;

	// How much force to use to push the ball bearing into the air
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=BallBearing)
	float JumpForce = 50.0f;

	// How much angular impulse power will be applied towards velocity when ball is not dashing and is in air
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=BallBearing)
	float InAirAngularImpulsePower = 1750.0f;

	// How much force to use to have the ball bearing dash
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BallBearing|Dash")
	float DashTime = 0.1f;

	// How much force to use to have the ball bearing dash
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BallBearing|Dash")
	float DashForce = 150.0f;

	// How much of a angular impulse will be applied when dashed in air
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BallBearing|Dash")
	float DashAngularImpulsePower = 10000000.0f;

	// Timer used to control the dashing of the ball bearing
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BallBearing|Dash")
	float DashCoolDown = 0.0f;

	// The vfx that plays when dashed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BallBearing|Dash")
	UParticleSystem* DashVfx;

protected:
	// Control the movement of the ball bearing, called every frame.
	virtual void Tick(float deltaSeconds) override;

	// Called to bind functionality to input.
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override
	{
		Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

		ChangeBallBearingState(Grounded);
	}

private:
	// Player ball bearing state
	EPlayerBallBearingState CurrentPlayerBallBearingState = InAir;

	// Change player ball bearing state if not dashing.
	void ChangeBallBearingState(const EPlayerBallBearingState WantedPlayerBallBearingState, const bool overrideDashing = false)
	{
		if (CurrentPlayerBallBearingState == Dashing && !overrideDashing)return;

		CurrentPlayerBallBearingState = WantedPlayerBallBearingState;
	}

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override
	{
		UE_LOG(LogTemp, Warning, TEXT("collision exit"));
	}

	// Move the ball bearing with the given force longitudinally on the X axis.
	void MoveLongitudinally(float value)
	{
		InputLongitude = value;
	}

	// Move the ball bearing with the given force longitudinally on the Y axis.
	void MoveLaterally(float value)
	{
		InputLatitude = value;
	}

	float GetMaximumSpeed() const
	{
		return MaximumSpeed * 100.0f;
	}

	// Have the ball bearing perform a jump
	void Jump();

	// Have the ball bearing perform a dash
	void Dash();

	// Player can dash if true
	bool bCanDash = true;

	// dash duration timer and dash cooldown timer
	FTimerHandle DashTimer, DashCooldownTimer;

	// for critically damped interpolation
	FVector DashVelocity = FVector::ZeroVector;
	FVector* DashVelocityPtr = &DashVelocity;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* DashVfxComponent = nullptr;

	// The current longitude input received from the player
	float InputLongitude = 0.0f;

	// The current latitude input received from the player
	float InputLatitude = 0.0f;

	// Get input vector based on InputLongitude and InputLatitude with 0 z
	FVector GetInputVector() const
	{
		auto normalizedInputVector = FVector(InputLongitude, InputLatitude, 0.0f);
		normalizedInputVector.Normalize();
		return normalizedInputVector;
	}

	// Get ball mesh velocity without upwards axis
	FVector GetVelocityWithoutUpwards(bool normalize = false) const
	{
		auto velocity = BallMesh->GetComponentVelocity();
		velocity.Z = 0;

		if (normalize)
		{
			velocity.Normalize();
		}

		return velocity;
	}

	friend class ABallBearingHUD;
};
