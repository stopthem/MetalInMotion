// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GameModeBase.h"
#include "MetalInMotionGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class METALINMOTION_API AMetalInMotionGameModeBase : public AGameMode
{
	GENERATED_BODY()

public:

	// Construct the game mode, assigning a debugging HUD class.
	AMetalInMotionGameModeBase();
};
