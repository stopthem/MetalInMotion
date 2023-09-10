// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBallBearingController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerBallBearing.h"

// bind input actions
void APlayerBallBearingController::BeginPlay()
{
	Super::BeginPlay();

	// // Get the local player subsystem
	const auto enhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	// Clear out existing mapping, and add our mapping
	enhancedInputSubsystem->ClearAllMappings();
	enhancedInputSubsystem->AddMappingContext(InputMappingContext, 0);

	// Get the EnhancedInputComponent
	const auto enhancedInputComponent = Cast<UEnhancedInputComponent>(GetPawn()->InputComponent);

	// Get player
	const auto playerBallBearing = Cast<APlayerBallBearing>(GetPawn());

	// bind actions
	enhancedInputComponent->BindAction(InputMove, ETriggerEvent::Triggered, playerBallBearing, &APlayerBallBearing::Move);
	enhancedInputComponent->BindAction(InputDash, ETriggerEvent::Triggered, playerBallBearing, &APlayerBallBearing::Dash);
	enhancedInputComponent->BindAction(InputJump, ETriggerEvent::Triggered, playerBallBearing, &APlayerBallBearing::Jump);
}
