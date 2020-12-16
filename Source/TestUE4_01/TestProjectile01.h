#pragma once

#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/ArrowComponent.h"
#include "TestProjectileSetting.h"
#include "TestProjectile01.generated.h"

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
    void InitProjectile(FVector& InPosition, FVector& InDirection, float InScale, bool bIsHitReflect = false, float InTimeDestroy = DEFAULT_DESTROY);

private:
    void UpdateLocation(FVector LocationCurrent, float DeltaTime);
    
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
