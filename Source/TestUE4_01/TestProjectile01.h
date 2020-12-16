#pragma once

#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/ArrowComponent.h"
#include "TestProjectile01.generated.h"

#define DEFAULT_DESTROY 9.0f
#define DEFAULT_SPEED 1.0f

class UBoxComponent;

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
    UBoxComponent* CollisionComponent;

private:
    float Speed = 0.0f;
    FVector PositionStart;
    FVector Direction;
    FQuat Rotation;

    float TimeSpawned = 0.0f;
    float TimeDestroyForce = 0.0f;
    bool IsHitReflect = false;

public:
    void InitProjectile(FVector& InPosition, FVector& InDirection, float InScale, float InTimeDestroy = DEFAULT_DESTROY, bool bIsHitReflect = false);

private:
    void UpdateLocation(FVector LocationCurrent);
    
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
