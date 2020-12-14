#include "TestProjectile01.h"
#include "Components/SphereComponent.h"

ATestProjectile01::ATestProjectile01()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    CollisionComponent->InitSphereRadius(15.0f);
    RootComponent = CollisionComponent;
}

void ATestProjectile01::BeginPlay()
{
    Super::BeginPlay();

}

void ATestProjectile01::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}
