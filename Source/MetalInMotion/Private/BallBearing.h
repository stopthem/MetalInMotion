// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BallBearing.generated.h"

/**
 * Main ball bearing class, derived from pawn but with no input and no camera.
 */
UCLASS()
class METALINMOTION_API ABallBearing : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABallBearing();

	// The static mesh that represents the ball bearing.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=BallBearing)
	UStaticMeshComponent* BallMesh = nullptr;

	// Is the ball bearing attractive to magnets?
	bool Magnetized = true;

	// Reset location of this ball bearings location to start location
	UFUNCTION(BlueprintCallable, Category="Ball Bearing")
	void ResetLocation() const
	{
		BallMesh->SetWorldLocation(InitialLocation + FVector(0.0f, 0.0f, 150.0f));
		BallMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
		BallMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	}

protected:
	// Called when game starts or when spawned
	virtual void BeginPlay() override;

private:
	/**
	 * @brief The initial location of the ball bearing at game start.
	 */
	FVector InitialLocation = FVector::ZeroVector;

	// Allow the ball bearing HUD unfettered access to this class.
	friend class ABallBearingHUD;
};
