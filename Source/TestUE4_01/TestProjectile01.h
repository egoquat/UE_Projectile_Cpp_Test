#pragma once

#include "GameFramework/Actor.h"
#include "TestProjectile01.generated.h"

class USphereComponent;

UCLASS()
class ATestProjectile01 : public AActor
{
    GENERATED_BODY()

public:
    // �� ���� ������Ƽ�� �⺻���� �����մϴ�.
    ATestProjectile01();

protected:
    // ���� ���� �Ǵ� ������ ȣ��˴ϴ�.
    virtual void BeginPlay() override;

public:
    // �� ������ ȣ��˴ϴ�.
    virtual void Tick(float DeltaSeconds) override;

    // ��ü �ݸ��� ������Ʈ�Դϴ�.
    UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
    USphereComponent* CollisionComponent;
};
