// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BallBearing.h"
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
	void SetSpringArm();
	void SetCamera();

	// Sets default values for this pawn's properties.
	APlayerBallBearing();

	// Spring arm for positioning the camera above the ball bearing.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=BallBearing)
	USpringArmComponent* SpringArm = nullptr;

	// Camera to view the ball bearing.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=BallBearing)
	UCameraComponent* Camera = nullptr;

	// How much force to use to push the ball bearing around.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=BallBearing)
	float ControllerForce = 250.0f;

	// How much force to use to push the ball bearing into the air.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=BallBearing)
	float JumpForce = 50.0f;

	// How much force to use to have the ball bearing dash.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=BallBearing)
	float DashForce = 150.0f;

	// The maximum speed in meters per second
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=BallBearing)
	float MaximumSpeed = 4.0f;

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

	// Have the ball bearing perform a jump
	void Jump();

	// Have the ball bearing perform a dash
	void Dash();

	// The current longitude input received from the player.
	float InputLongitude = 0.0f;

	// The current latitude input received from the player.
	float InputLatitude = 0.0f;

	// Timer used to control the dashing of the ball bearing.
	float DashTimer = 0.0f;

	friend class ABallBearingHUD;
};
