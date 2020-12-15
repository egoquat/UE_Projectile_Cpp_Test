#include "TestProjectile01.h"
#include "TestUE4_01Character.h"
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
    if (FPlatformTime::Seconds() >= TimeDestroyForce)
    {
        ATestUE4_01Character::AddDestoryRequest(this);
        ArrowComponent->bHiddenInGame = true;
        return;
    }

    Super::Tick(DeltaTime);
    UpdateLocation(GetTransform().GetLocation());
}

void ATestProjectile01::InitProjectile(ATestUE4_01Character* InOwner, FVector& InPosition, FVector& InDirection, float InSpeed, float InTimeDestroy)
{
    Direction = InDirection;
    Rotation = Direction.ToOrientationQuat();
    Speed = InSpeed;
    TimeSpawned = FPlatformTime::Seconds();
    TimeDestroyForce = TimeSpawned + InTimeDestroy;
    PositionStart = InPosition;
    OwnerTest = InOwner;
    SetActorLocationAndRotation(PositionStart, Rotation, false, 0, ETeleportType::None);
}

void ATestProjectile01::UpdateLocation(FVector LocationCurrent)
{
    FVector NewLocation = LocationCurrent + (Direction * Speed);
    SetActorLocationAndRotation(NewLocation, Rotation, false, 0, ETeleportType::None);
}
