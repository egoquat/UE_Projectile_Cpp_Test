// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TestProjectile.h"
#include "TestProjectileSetting.h"
#include "TestUE4_01Character.generated.h"

UENUM()
enum class ETestKey : uint8
{
	Q = 0,
	W,
	E,
	R,
	A
};

UENUM()
enum class ESkill : uint8
{
	Skill01 = 0,
	Skill02,
	Skill03,
	Skill04,
	Skill05,
	Skill06
};

struct FTestKeyEvent
{
	ETestKey Key;
	bool IsPressed = false;
	bool IsReleased = false;
	bool IsEvented = false;
	float TimePressStart = -1.0f;
	float TimePressRelease = 0.0f;
	FTestKeyEvent(ETestKey key) { Key = key; }

private:
	void Reset();

public:
	float GetElapsePressed();

public:
	void StartPress();
	float EndPress();
	void TickPost();
};

struct FKeyAction
{
	ESkill TypeSkill;
	TFunction<void()> Action = nullptr;
	TFunction<bool(float)> Cond = nullptr;
	TFunction<float()> OnProgress = nullptr;
	float TimeLastCall = 0.0f;
	float GetProgress();
	FKeyAction(ESkill typeSkill, TFunction<void()> action, TFunction<bool(float)> cond, TFunction<float()> onprogress = nullptr);
	void TickAction(float deltaSec);
};

UCLASS(config=Game)
class ATestUE4_01Character : public ACharacter
{
	GENERATED_BODY()

	/** Side view camera */
	UPROPERTY()
	class UCameraComponent* SideViewCameraComponent;

	UPROPERTY()
	class USpringArmComponent* CameraBoom;

	UPROPERTY()
	class UCharacterMovementComponent* Movement;

	static TSet<ATestProjectile*> DestroyProjectiles;

public:
	static TArray<AActor*> FirstActors;

public:
	ATestUE4_01Character();

	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

private:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

private:
	template<class T>
	T* SpawnProjectileByActor(float InScale = 1.0f, float InAngle = 0.0f, bool bIsHitReflect = false, float InTimeDestroy = DEFAULT_DESTROY, FColor InColor = FColor::Red);

	template<class T>
	void SpawnProjectileQueued(const FTransform& InTM, const FVector& InPosRelative, float InScale = 1.0f, float InAngle = 0.0f, bool bIsHitReflect = false, float InTimeDestroy = DEFAULT_DESTROY, FColor InColor = FColor::Red);

	template<class T>
	T* SpawnProjectile(const FTransform& InTM, const FVector& InPosRelative, float InScale = 1.0f, float InAngle = 0.0f, bool bIsHitReflect = false, float InTimeDestroy = DEFAULT_DESTROY, FColor InColor = FColor::Red);

private:
	void StartSkill_1();
	void StartSkill_2();
	void StartSkill_3();
	void StartSkill_4();
	void StartSkill_5();
	void StartSkill_6();

private:
	void MoveRight(float Val);

	void KeyQPressedStart();
	void KeyQPressedEnd();

	void KeyWPressedStart();
	void KeyWPressedEnd();

	void KeyEPressedStart();
	void KeyEPressedEnd();

	void KeyAPressedStart();
	void KeyAPressedEnd();

	void KeyRPressedEnd();

public:
	static void AddDestroyRequest(ATestProjectile* TestProjectile);
	static int GetProgressSkill_2();
};
