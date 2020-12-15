#pragma once

#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/ArrowComponent.h"
#include "TestProjectile01.generated.h"

#define DEFAULT_TIME_DESTROY 9.0f

class USphereComponent;

UCLASS()
class ATestProjectile01 : public AActor
{
    GENERATED_BODY()

public:
    ATestProjectile01();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY()
    UArrowComponent* ArrowComponent;

    UPROPERTY()
    USphereComponent* CollisionComponent;

private:
    float Speed = 0.0f;
    FVector PositionStart;
    FVector Direction;
    FQuat Rotation;

    float TimeSpawned = 0.0f;
    float TimeDestroyForce = 0.0f;
    class ATestUE4_01Character* OwnerTest = nullptr;

public:
    void InitProjectile(ATestUE4_01Character* InOwner, FVector& InPosition, FVector& InDirection, float InSpeed, float InTimeDestroy = DEFAULT_TIME_DESTROY);

private:
    void UpdateLocation(FVector LocationCurrent);
};
