#include "TestProjectileSpline.h"

static float MIN_RADIUS_PROJECTILE = 15.0f;

ATestProjectileSpline::ATestProjectileSpline()
{
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
}

void ATestProjectileSpline::InitProjectile(FVector& InPosition, FVector& InDirection, float InScale, bool bIsHitReflect, float InTimeDestroy, FColor InColor)
{
	Super::InitProjectile(InPosition, InDirection, InScale, bIsHitReflect, InTimeDestroy, InColor);
	
}

void ATestProjectileSpline::UpdateLocation(FVector LocationCurrent, float DeltaTime)
{
	Super::UpdateLocation(LocationCurrent, DeltaTime);

}
