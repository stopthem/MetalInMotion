// Fill out your copyright notice in the Description page of Project Settings.


#include "BallBearingHUD.h"
#include "PlayerBallBearing.h"

void ABallBearingHUD::DrawHUD()
{
	Super::DrawHUD();

	const APlayerBallBearing* ballBearing = Cast<APlayerBallBearing>(GetOwningPawn());

	if (!ballBearing)return;

	AddFloat(L"Speed", ballBearing->GetVelocity().Size() / 100.0f);
	AddFloat(L"Input Latitude", ballBearing->InputVector.Y);
	AddFloat(L"Input Longitude", ballBearing->InputVector.X);
	AddBool(L"Is Grounded", ballBearing->IsGrounded());
	AddBool(L"Can Dash", ballBearing->bCanDash);
	AddText(L"Player Ball Bearing State", FText::FromString(UEnum::GetValueAsName(ballBearing->CurrentPlayerBallBearingState).ToString()));
}
