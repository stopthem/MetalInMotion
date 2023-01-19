// Fill out your copyright notice in the Description page of Project Settings.


#include "BallBearingHUD.h"
#include "PlayerBallBearing.h"

void ABallBearingHUD::DrawHUD()
{
	Super::DrawHUD();

	const APlayerBallBearing* ballBearing = Cast<APlayerBallBearing>(GetOwningPawn());

	if (!ballBearing)return;

	AddFloat(L"Speed", ballBearing->GetVelocity().Size() / 100.0f);
	AddFloat(L"Dash Timer", ballBearing->DashTimer);
	AddFloat(L"Input Latitude", ballBearing->InputLatitude);
	AddFloat(L"Input Longitude", ballBearing->InputLongitude);
	AddBool(L"In Contact", ballBearing->InContact);
}
