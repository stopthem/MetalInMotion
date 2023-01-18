// Fill out your copyright notice in the Description page of Project Settings.


#include "BallBearing.h"

// Sets default values
ABallBearing::ABallBearing()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ball Mesh"));
	BallMesh->SetSimulatePhysics(true);
	SetRootComponent(BallMesh);
}

void ABallBearing::BeginPlay()
{
	BallMesh->SetLinearDamping(0.5f);
	BallMesh->SetAngularDamping(0.5f);

	InitialLocation = BallMesh->GetComponentLocation();
}


void ABallBearing::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	InContact = false;
}
