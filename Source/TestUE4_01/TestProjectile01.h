#pragma once

#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/ArrowComponent.h"
#include "TestProjectile01.generated.h"

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
    float TimeDestroyForce = -1.0f;

public:
    void InitProjectile(FVector& InPosition, FVector& InDirection, float InSpeed, float InTimeDestroy = -1.0f);

private:
    void UpdateLocation(FVector LocationCurrent);
    bool IsTimeDestoryForce() { return 0.0f < TimeDestroyForce; }
};
