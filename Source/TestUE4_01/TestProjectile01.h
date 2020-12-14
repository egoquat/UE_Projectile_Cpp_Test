#pragma once

#include "GameFramework/Actor.h"
#include "TestProjectile01.generated.h"

class USphereComponent;

UCLASS()
class ATestProjectile01 : public AActor
{
    GENERATED_BODY()

public:
    // 이 액터 프로퍼티의 기본값을 설정합니다.
    ATestProjectile01();

protected:
    // 게임 시작 또는 스폰시 호출됩니다.
    virtual void BeginPlay() override;

public:
    // 매 프레임 호출됩니다.
    virtual void Tick(float DeltaSeconds) override;

    // 구체 콜리전 컴포넌트입니다.
    UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
    USphereComponent* CollisionComponent;
};
