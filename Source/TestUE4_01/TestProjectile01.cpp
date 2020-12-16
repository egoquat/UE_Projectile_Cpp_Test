#include "TestProjectile01.h"
#include "TestUE4_01Character.h"
#include "Components/BoxComponent.h"

ATestProjectile01::ATestProjectile01()
{
    PrimaryActorTick.bCanEverTick = true;

    ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
    ArrowComponent->bHiddenInGame = false;
    ArrowComponent->bUseAttachParentBound = false;
    RootComponent = ArrowComponent;

    CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    CollisionComponent->SetVisibleFlag(true);
    CollisionComponent->bDynamicObstacle = true;
    CollisionComponent->bHiddenInGame = false;
    CollisionComponent->InitBoxExtent(FVector(45, 7, 7));
    CollisionComponent->AddRelativeLocation(FVector(40, 0, 0));
    CollisionComponent->bUseAttachParentBound = true;
    CollisionComponent->SetupAttachment(ArrowComponent);
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

void ATestProjectile01::InitProjectile(FVector& InPosition, FVector& InDirection, float InScale, float InTimeDestroy, bool bIsHitReflect)
{
    Direction = InDirection;
    Rotation = Direction.ToOrientationQuat();
    Speed = DEFAULT_SPEED;
    TimeSpawned = FPlatformTime::Seconds();
    TimeDestroyForce = TimeSpawned + InTimeDestroy;
    PositionStart = InPosition;
    
    IsHitReflect = bIsHitReflect;
    CollisionComponent->SetCollisionEnabled(true == bIsHitReflect ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
    CollisionComponent->OnComponentHit.AddDynamic(this, &ATestProjectile01::OnHit);

    ArrowComponent->ArrowSize = InScale;
    SetActorScale3D(FVector(InScale));
    SetActorLocationAndRotation(PositionStart, Rotation, false, 0, ETeleportType::None);
}

void ATestProjectile01::UpdateLocation(FVector LocationCurrent)
{
    FVector NewLocation = LocationCurrent + (Direction * Speed);
    SetActorLocationAndRotation(NewLocation, Rotation, false, 0, ETeleportType::None);
}

void ATestProjectile01::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    GWarn->Logf(ELogVerbosity::Display, TEXT("ATestProjectile01::OnHit/%d"), Hit.Time);
    Direction = Direction - ((FVector::DotProduct(Direction, Hit.Normal) * 2) * Hit.Normal);
}
