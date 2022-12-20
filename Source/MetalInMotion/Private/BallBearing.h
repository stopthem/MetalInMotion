// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BallBearing.generated.h"

UCLASS()
class METALINMOTION_API ABallBearing : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABallBearing();

	// The static mesh that represents the ball bearing.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=BallBearing)
	UStaticMeshComponent* BallMesh;

	// Is the ball bearing attractive to magnets?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=BallBearing)
	float Magnetized = 1.0f;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// Allow the ball bearing HUD unfettered access to this class.
	friend class ABallBearingHUD;
};
