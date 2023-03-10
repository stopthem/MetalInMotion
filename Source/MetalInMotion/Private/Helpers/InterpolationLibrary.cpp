#include "Helpers/InterpolationLibrary.h"
#include "Kismet/KismetMathLibrary.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Critically Damped Spring Interpolations (ie: Similar to Unity's SmoothDamp, but will less operations)
// Inspired from Keijiro's code: https://github.com/keijiro/SmoothingTest
// Math reference: http://mathproofs.blogspot.jp/2013/07/critically-damped-spring-smoothing.html

FVector UInterpolationLibrary::VectorSpringInterpCD(FVector Current, FVector Target, FVector& Velocity, float DeltaTime, float InterpSpeed, float MaxVelocity)
{
	const FVector n1 = Velocity - (Current - Target) * (InterpSpeed * InterpSpeed * DeltaTime);
	const float n2 = 1.f + InterpSpeed * DeltaTime;
	if (MaxVelocity > 0.f)
	{
		Velocity = (n1 / (n2 * n2)).GetClampedToMaxSize(MaxVelocity);
	}
	else
	{
		Velocity = n1 / (n2 * n2);
	}
	return Current + Velocity * DeltaTime;
}

float UInterpolationLibrary::FloatSpringInterpCD(float Current, float Target, float& Velocity, float DeltaTime, float InterpSpeed, float MaxVelocity)
{
	const float n1 = Velocity - (Current - Target) * (InterpSpeed * InterpSpeed * DeltaTime);
	const float n2 = 1.f + InterpSpeed * DeltaTime;

	Velocity = (MaxVelocity > 0.f) ? FMath::Min(n1 / (n2 * n2), MaxVelocity) : n1 / (n2 * n2);

	return Current + Velocity * DeltaTime;
}

FRotator UInterpolationLibrary::RotatorSpringInterpCD(FRotator Current, FRotator Target, FVector4& Velocity, float DeltaTime, float InterpSpeed, float MaxVelocity)
{
	return QuatSpringInterpCD(Current.Quaternion(), Target.Quaternion(), Velocity, DeltaTime, InterpSpeed, MaxVelocity).Rotator();
}

FQuat UInterpolationLibrary::QuatSpringInterpCD(FQuat Current, FQuat Target, FVector4& Velocity, float DeltaTime, float InterpSpeed, float MaxVelocity)
{
	// Here would it be better to make operations directly on FQuat? 
	// I can't find FQuat operators code to check, so I prefer those conversions...
	FVector4 currentVector = QuatToVector4(Current);
	FVector4 targetVector = QuatToVector4(Target);

	// We can use either of vtarget/-vtarget. Use closer one. 
	// If using FQuat, might FQuat::Squad() be usesul here?
	if (Dot4(currentVector, targetVector) < 0.f) targetVector = -targetVector;

	const FVector4 n1 = Velocity - (currentVector - targetVector) * (InterpSpeed * InterpSpeed * DeltaTime);
	const float n2 = 1.f + InterpSpeed * DeltaTime;

	if (MaxVelocity > 0.f)
	{
		Velocity = ClampVector4(n1 / (n2 * n2), MaxVelocity);
	}
	else
	{
		Velocity = n1 / (n2 * n2);
	}
	// Apply delta on current
	currentVector = (currentVector + Velocity * DeltaTime);

	// Normalizing gave odd results, it looks fine this way but don't ask me why...
	return FQuat(currentVector.X, currentVector.Y, currentVector.Z, currentVector.W);
}
