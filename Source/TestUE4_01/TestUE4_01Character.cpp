// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestUE4_01Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Slate/Public/Framework/Commands/InputChord.h"
#include "Runtime/InputCore/Classes/InputCoreTypes.h"

//////////////////////////////////////////////////////////////////////////
// Input Setting

UENUM()
namespace EKeyTest
{
	enum Type
	{
		Q = 0,
		W,
		A
	};
}

struct FKeyTest
{
	EKeyTest::Type Key;
	bool IsPressed = false;
	float TimePressStart = -1.0f;
	FKeyTest(EKeyTest::Type key) { Key = key; }
	void StartPress() 
	{
		if (true == IsPressed)
			return;
		IsPressed = true;
		TimePressStart = FPlatformTime::Seconds();
	}
	float EndPress()
	{
		if (false == IsPressed) return -1.0f;
		IsPressed = false;
		float elapsed = FPlatformTime::Seconds() - TimePressStart;
		GWarn->Logf(ELogVerbosity::Display, TEXT(" EndPressKey/%d/Elapsed:%f"), Key, elapsed);
		return elapsed;
	}
};

static TArray<FKeyTest> KeyTestEvents;

ATestUE4_01Character::ATestUE4_01Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Rotation of the character should not affect rotation of boom
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->SocketOffset = FVector(0.f,0.f,75.f);
	CameraBoom->SetRelativeRotation(FRotator(0.f,180.f,0.f));

	// Create a camera and attach to boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	SideViewCameraComponent->bUsePawnControlRotation = false; // We don't want the controller rotating the camera

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Face in the direction we are moving..
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->GravityScale = 2.f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MaxFlySpeed = 600.f;

	if (KeyTestEvents.Num() <= 0)
	{
		KeyTestEvents.Add(FKeyTest(EKeyTest::Q));
		KeyTestEvents.Add(FKeyTest(EKeyTest::W));
		KeyTestEvents.Add(FKeyTest(EKeyTest::A));
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATestUE4_01Character::Tick(float DeltaSeconds)
{

}

void ATestUE4_01Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATestUE4_01Character::MoveRight);

	// projectile key bindings.
	PlayerInputComponent->BindKey(EKeys::Q, EInputEvent::IE_Pressed, this, &ATestUE4_01Character::KeyQPressedStart);
	PlayerInputComponent->BindKey(EKeys::Q, EInputEvent::IE_Released, this, &ATestUE4_01Character::KeyQPressedEnd);

	PlayerInputComponent->BindKey(EKeys::W, EInputEvent::IE_Pressed, this, &ATestUE4_01Character::KeyQPressedStart);
	PlayerInputComponent->BindKey(EKeys::W, EInputEvent::IE_Released, this, &ATestUE4_01Character::KeyQPressedEnd);

	PlayerInputComponent->BindKey(EKeys::A, EInputEvent::IE_Pressed, this, &ATestUE4_01Character::KeyQPressedStart);
	PlayerInputComponent->BindKey(EKeys::A, EInputEvent::IE_Released, this, &ATestUE4_01Character::KeyQPressedEnd);
}

void ATestUE4_01Character::MoveRight(float Value)
{
	// add movement in that direction
	AddMovementInput(FVector(0.f,-1.f,0.f), Value);
}

void ATestUE4_01Character::KeyQPressedStart()
{
	KeyTestEvents[EKeyTest::Q].StartPress();
}

void ATestUE4_01Character::KeyQPressedEnd()
{
	KeyTestEvents[EKeyTest::Q].EndPress();
}

void ATestUE4_01Character::KeyWPressedStart()
{
	KeyTestEvents[EKeyTest::W].StartPress();
}

void ATestUE4_01Character::KeyWPressedEnd()
{
	KeyTestEvents[EKeyTest::W].EndPress();
}

void ATestUE4_01Character::KeyAPressedStart()
{
	KeyTestEvents[EKeyTest::A].StartPress();
}

void ATestUE4_01Character::KeyAPressedEnd()
{
	KeyTestEvents[EKeyTest::A].EndPress();
}
