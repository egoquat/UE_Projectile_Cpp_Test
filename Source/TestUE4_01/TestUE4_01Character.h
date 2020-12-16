// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TestProjectile01.h"
#include "TestUE4_01Character.generated.h"

UENUM()
namespace ETestKey
{
	enum Type
	{
		Q = 0,
		W,
		A
	};
}

struct FTestKeyEvent
{
	ETestKey::Type Key;
	bool IsPressed = false;
	bool IsReleased = false;
	bool IsEvented = false;
	float TimePressStart = -1.0f;
	float TimePressRelease = 0.0f;
	FTestKeyEvent(ETestKey::Type key) { Key = key; }

private:
	void Reset()
	{
		IsEvented = IsReleased = false;
		TimePressRelease = 0.0f;
	}

public:
	float GetElapsePressed() { return FPlatformTime::Seconds() - TimePressStart; }

public:
	void StartPress()
	{
		Reset();
		if (true == IsPressed)
			return;
		IsPressed = true;
		TimePressStart = FPlatformTime::Seconds();
	}

	float EndPress()
	{
		if (false == IsPressed) return -1.0f;
		IsPressed = false;
		TimePressRelease = FPlatformTime::Seconds() - TimePressStart;
		GWarn->Logf(ELogVerbosity::Display, TEXT(" EndPressKey/%d/Elapsed:%f"), Key, TimePressRelease);
		IsReleased = true;
		return TimePressRelease;
	}

	void TickPost()
	{
		Reset();
	}
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

	static TSet<ATestProjectile01*> DestoryProjectiles;

public:
	ATestUE4_01Character();

	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

private:
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

private:
	void SpawnAProjectile(float InScale = 1.0f, float InAngle = 0.0f);

private:
	void StartSkill_1();
	void StartSkill_2();
	void StartSkill_3();
	void StartSkill_4();
	void StartSkill_5();

private:
	void MoveRight(float Val);
	void KeyQPressedStart();
	void KeyQPressedEnd();

	void KeyWPressedStart();
	void KeyWPressedEnd();

	void KeyAPressedStart();
	void KeyAPressedEnd();

public:
	static void AddDestoryRequest(ATestProjectile01* TestProjectile);
};
