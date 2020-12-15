#include "TestProjectile01.h"
#include "Components/SphereComponent.h"

ATestProjectile01::ATestProjectile01()
{
    PrimaryActorTick.bCanEverTick = true;

    ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
    ArrowComponent->bHiddenInGame = false;
    ArrowComponent->bUseAttachParentBound = false;
    FBoxSphereBounds Bound = ArrowComponent->CalcBounds(FTransform());
    RootComponent = ArrowComponent;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    CollisionComponent->InitSphereRadius(Bound.SphereRadius);
    CollisionComponent->SetVisibleFlag(true);
}

void ATestProjectile01::BeginPlay()
{
    Super::BeginPlay();
}

void ATestProjectile01::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateLocation(GetTransform().GetLocation());
}

void ATestProjectile01::InitProjectile(FVector& InPosition, FVector& InDirection, float InSpeed, float InTimeDestroy)
{
    Direction = InDirection;
    Rotation = Direction.ToOrientationQuat();
    Speed = InSpeed;
    TimeDestroyForce = InTimeDestroy;
    PositionStart = InPosition;
    SetActorLocationAndRotation(PositionStart, Rotation, false, 0, ETeleportType::None);
}

void ATestProjectile01::UpdateLocation(FVector LocationCurrent)
{
    FVector NewLocation = LocationCurrent + (Direction * Speed);
    SetActorLocationAndRotation(NewLocation, Rotation, false, 0, ETeleportType::None);
}
