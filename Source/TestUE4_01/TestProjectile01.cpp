#include "TestProjectile01.h"
#include "TestUE4_01Character.h"
#include "Components/BoxComponent.h"
#include "Engine/CollisionProfile.h"

TSet<ATestProjectile01*> ATestProjectile01::Projectiles;

ATestProjectile01::ATestProjectile01()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    CollisionComponent->SetVisibleFlag(true);
    CollisionComponent->bHiddenInGame = false;
    CollisionComponent->InitBoxExtent(FVector(45, 7, 7));
    CollisionComponent->AddRelativeLocation(FVector(DEFAULT_ARROW_LEN, 0, 0));
    //CollisionComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    //CollisionComponent->SetCollisionProfileName(TEXT("OverlapAll"));
    //CollisionComponent->SetCollisionProfileName(TEXT("BlockAll"));

    ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
    ArrowComponent->bHiddenInGame = false;
    ArrowComponent->bUseAttachParentBound = false;
    ArrowComponent->AddRelativeLocation(FVector(-45, 0, 0));
    ArrowComponent->SetupAttachment(CollisionComponent);

    RootComponent = CollisionComponent;
}

void ATestProjectile01::BeginPlay()
{
    Super::BeginPlay();
	Projectiles.Add(this);
}

void ATestProjectile01::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	Projectiles.Remove(this);
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
    UpdateLocation(GetTransform().GetLocation(), DeltaTime);
}

void ATestProjectile01::InitProjectile(FVector& InPosition, FVector& InDirection, float InScale, bool bIsHitReflect, float InTimeDestroy, FColor InColor)
{
    Direction = InDirection;
    Rotation = Direction.ToOrientationQuat();
    Speed = DEFAULT_SPEED;
    TimeSpawned = FPlatformTime::Seconds();
    TimeDestroyForce = TimeSpawned + InTimeDestroy;
    PositionStart = InPosition + (InDirection * DEFAULT_RELATIVE);
    
    IsHitReflect = bIsHitReflect;
    //CollisionComponent->SetCollisionEnabled(true == bIsHitReflect ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    //CollisionComponent->OnComponentHit.AddDynamic(this, &ATestProjectile01::OnHit);
    //CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ATestProjectile01::OnBeginOverlap);

	ArrowComponent->SetArrowColor(InColor);

    SetActorScale3D(FVector(InScale));
    SetActorLocationAndRotation(PositionStart, Rotation, false, 0, ETeleportType::None);
}

void ATestProjectile01::UpdateLocation(FVector LocationCurrent, float DeltaTime)
{
    FCollisionQueryParams TraceParams;
    TraceParams.AddIgnoredActor(this);
    TraceParams.AddIgnoredActors(ATestUE4_01Character::FirstActors);
    TraceParams.bTraceComplex = true;
    FHitResult Hit;
    bool Hitted = GWorld->LineTraceSingleByObjectType(Hit, LocationCurrent, LocationCurrent + (Direction * DEFAULT_ARROW_LEN), FCollisionObjectQueryParams(ECC_WorldStatic), TraceParams);
    if (true == Hitted && Hit.Actor.Get()->GetClass() != ATestProjectile01::StaticClass())
    {
		if (true == IsHitReflect)
		{
			GWarn->Logf(ELogVerbosity::Error, TEXT(" IsHitCheckForce/Reflect:%s/"), *Hit.GetActor()->GetName());
			Direction = Direction - ((FVector::DotProduct(Direction, Hit.Normal) * 2) * Hit.Normal);
			//Direction = FMath::GetReflectionVector(Direction, Hit.Normal);
		}
		else
		{
			ATestUE4_01Character::AddDestoryRequest(this);
			ArrowComponent->bHiddenInGame = true;
			return;
		}
    }

    FVector NewLocation = LocationCurrent + (Direction * (Speed * DeltaTime));
    Rotation = Direction.ToOrientationQuat();
    SetActorLocationAndRotation(NewLocation, Rotation, false, 0, ETeleportType::None);
}

void ATestProjectile01::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    GWarn->Logf(ELogVerbosity::Error, TEXT("ATestProjectile01::OnHit/%s"), *OtherActor->GetName());
    Direction = Direction - ((FVector::DotProduct(Direction, Hit.Normal) * 2) * Hit.Normal);
}

void ATestProjectile01::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    GWarn->Logf(ELogVerbosity::Error, TEXT("ATestProjectile01::OnBeginOverlap/Other:%s"), *Other->GetName());
    Direction = Direction - ((FVector::DotProduct(Direction, SweepResult.Normal) * 2) * SweepResult.Normal);
}
