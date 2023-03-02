// Fill out your copyright notice in the Description page of Project Settings.


#include "BallBearing.h"

#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ABallBearing::ABallBearing()
{
	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ball Mesh"));
	BallMesh->SetSimulatePhysics(true);
	SetRootComponent(BallMesh);
}

void ABallBearing::BeginPlay()
{
	Super::BeginPlay();

	BallMesh->SetLinearDamping(0.5f);
	BallMesh->SetAngularDamping(0.5f);

	InitialLocation = BallMesh->GetComponentLocation();
}
