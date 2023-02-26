// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerBallBearingController.generated.h"

class UInputAction;
class UInputMappingContext;
/**
 * A class for input handling on player
 */
UCLASS()
class METALINMOTION_API APlayerBallBearingController : public APlayerController
{
	GENERATED_BODY()
	
public:
	// the mapping context of input actions
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Enhanced Input")
	UInputMappingContext* InputMappingContext;

	// Dash input action
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Enhanced Input|Input Actions")
	UInputAction* InputDash;

	// Move input action
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Enhanced Input|Input Actions")
	UInputAction* InputMove;

	// Jump input action
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Enhanced Input|Input Actions")
	UInputAction* InputJump;

private:
	// bind input actions
	virtual void BeginPlay() override;
};
