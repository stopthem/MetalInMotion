// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BallBearing.h"
#include "FCTweenInstance.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PlayerBallBearing.generated.h"

/**
 * 
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

	// How much force to use to push the ball bearing into the air
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=BallBearing)
	float JumpForce = 50.0f;

	// How much angular impulse power will be applied towards velocity when ball is not dashing and is in air
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=BallBearing)
	float InAirAngularImpulsePower = 1750.0f;

	// How much force to use to have the ball bearing dash
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BallBearing|Dash")
	float DashForce = 150.0f;

	// How much of a angular impulse will be applied when dashed in air
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BallBearing|Dash")
	float DashAngularImpulsePower = 10000000.0f;

	//Ease of the dash
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BallBearing|Dash")
	EFCEase DashEase = EFCEase::OutSine;

	// Timer used to control the dashing of the ball bearing
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BallBearing|Dash")
	float DashTimer = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BallBearing|Dash")
	UParticleSystem* DashVfx;

protected:
	// Control the movement of the ball bearing, called every frame.
	virtual void Tick(float deltaSeconds) override;

	// Called to bind functionality to input.
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
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

	// The current longitude input received from the player.
	float InputLongitude = 0.0f;

	// The current latitude input received from the player.
	float InputLatitude = 0.0f;

	// Get input vector based on InputLongitude and InputLatitude with 0 z.
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

	bool bCanDash = true;
	FCTweenInstance* DashTimerTween = nullptr;
	float BrakingRatio = 0.0f;

	friend class ABallBearingHUD;
};
