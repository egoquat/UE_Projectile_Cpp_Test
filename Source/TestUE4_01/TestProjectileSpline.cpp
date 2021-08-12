#include "TestProjectileSpline.h"

ATestProjectileSpline::ATestProjectileSpline()
{
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
}

void ATestProjectileSpline::InitProjectile(const FVector& InPosition, FVector& InDirection, float InScale, bool bIsHitReflect, float InTimeDestroy, FColor InColor)
{
	SplineComponent->AddSplineWorldPoint(InPosition);
	const int countIteration = 10;
	const float distance = 1000.0f, distanceUnit = distance / (float)countIteration, distanceWave = 50.0f;
	for (int i = 0; i < countIteration; ++i)
	{
		float currentDistance = distanceUnit * i;
		FVector currentPosition = InPosition + (InDirection * currentDistance) + ((0 == i % 2)? distanceWave : -distanceWave);
		SplineComponent->AddSplineWorldPoint(currentPosition);
	}

	TimeCumulative = 0.0f;

	Super::InitProjectile(InPosition, InDirection, InScale, bIsHitReflect, InTimeDestroy, InColor);	
}

void ATestProjectileSpline::UpdateLocation(const FVector& LocationCurrent, float DeltaTime)
{
	Super::UpdateLocation(LocationCurrent, DeltaTime);
	TimeCumulative = TimeCumulative + DeltaTime;
}
