// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestUE4_01Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Slate/Public/Framework/Commands/InputChord.h"
#include "Runtime/InputCore/Classes/InputCoreTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "TestProjectile01.h"
#include "TestUE4_01UI.h"

TSet<ATestProjectile01*> ATestUE4_01Character::DestoryProjectiles;
TArray<AActor*> ATestUE4_01Character::FirstActors;

//////////////////////////////////////////////////////////////////////////
// Settings
UENUM()
namespace ESkill
{
	enum Type
	{
		_1 = 0,
		_2,
		_3,
		_4,
		_5
	};
};

struct FKeyAction
{
	ESkill::Type TypeSkill;
	TFunction<void()> Action = nullptr;
	TFunction<bool(float)> Cond = nullptr;
	TFunction<float()> OnProgress = nullptr;

	float GetProgress()
	{
		if (nullptr == OnProgress) return 1.0f;
		return OnProgress();
	}

	FKeyAction(ESkill::Type typeSkill, TFunction<void()> action, TFunction<bool(float)> cond, TFunction<float()> onprogress = nullptr)
	{
		TypeSkill = typeSkill;
		Action = action;
		Cond = cond;
		OnProgress = onprogress;
	}

	void TickAction(float deltaSec)
	{
		if (false == Cond(deltaSec))
			return;

		Action();
	}
};

static TArray<FTestKeyEvent> TestKeys;
static TArray<FKeyAction> TestKeyActions;

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
	Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = true; // Face in the direction we are moving..
	Movement->RotationRate = FRotator(0.0f, 720.0f, 0.0f); // ...at this rotation rate
	Movement->GravityScale = 2.f;
	Movement->AirControl = 0.80f;
	Movement->JumpZVelocity = 1000.f;
	Movement->GroundFriction = 3.f;
	Movement->MaxWalkSpeed = 600.f;
	Movement->MaxFlySpeed = 600.f;

	// Init action skill
	if (TestKeys.Num() <= 0)
	{
		TestKeys.Add(FTestKeyEvent(ETestKey::Q));
		TestKeys.Add(FTestKeyEvent(ETestKey::W));
		TestKeys.Add(FTestKeyEvent(ETestKey::A));

		TFunction<bool(float)> condition = nullptr;
		TFunction<float()> onprogress = nullptr;
		condition = [&](float deltaSec)
		{
			FTestKeyEvent& keyQ = TestKeys[ETestKey::Q];
			return (true == keyQ.IsReleased && 3.0f > keyQ.TimePressRelease);
		};
		TestKeyActions.Add(FKeyAction(ESkill::_1, [&]() {StartSkill_1(); }, condition));

		condition = [&](float deltaSec)
		{
			FTestKeyEvent& keyQ = TestKeys[ETestKey::Q];
			return (true == keyQ.IsReleased && 3.0f <= keyQ.TimePressRelease);
		};
		onprogress = [&]()
		{
			FTestKeyEvent& keyQ = TestKeys[ETestKey::Q];
			float progress = 0.0f;
			if (true == keyQ.IsPressed && keyQ.GetElapsePressed() > 0.0f)
			{
				progress = (keyQ.GetElapsePressed()) / (3.0f);
				progress = FMath::Clamp(progress, 0.0f, 1.0f);
			}
			return progress;
		};
		TestKeyActions.Add(FKeyAction(ESkill::_2, [&]() {StartSkill_2(); }, condition, onprogress));

		condition = [&](float deltaSec)
		{
			FTestKeyEvent& keyQ = TestKeys[ETestKey::Q];
			FTestKeyEvent& keyW = TestKeys[ETestKey::W];
			return (true == keyQ.IsPressed && 1.0f >= keyQ.GetElapsePressed() && true == keyW.IsPressed);
		};
		TestKeyActions.Add(FKeyAction(ESkill::_3, [&]() {StartSkill_3(); }, condition));

		condition = [&](float deltaSec)
		{
			FTestKeyEvent& keyQ = TestKeys[ETestKey::Q];
			FTestKeyEvent& keyW = TestKeys[ETestKey::W];
			return (false == keyQ.IsPressed && true == keyW.IsReleased);
		};
		TestKeyActions.Add(FKeyAction(ESkill::_4, [&]() {StartSkill_4(); }, condition));

		condition = [&](float deltaSec)
		{
			FTestKeyEvent& keyA = TestKeys[ETestKey::A];
			return (true == keyA.IsReleased);
		};
		TestKeyActions.Add(FKeyAction(ESkill::_5, [&]() {StartSkill_5(); }, condition));
	}
}

void ATestUE4_01Character::BeginPlay()
{
	FTestUE4_01UI::InitTestUI();

	Super::BeginPlay();
	FirstActors.Add(this);

	APlayerController* PlayerController = GEngine->GetFirstLocalPlayerController(GWorld);
	if (nullptr != PlayerController)
	{
		APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
		CameraManager->SetFOV(90);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATestUE4_01Character::Tick(float DeltaSeconds)
{
	for (int i = 0; i < TestKeyActions.Num(); ++i)
	{
		FKeyAction& keyAction = TestKeyActions[i];
		keyAction.TickAction(DeltaSeconds);
	}

	for (int i = 0; i < TestKeys.Num(); ++i)
	{
		FTestKeyEvent& testkey = TestKeys[i];
		testkey.TickPost();
	}

	TSet<ATestProjectile01*> Destroys;
	for (ATestProjectile01* projectile : DestoryProjectiles)
	{
		GWorld->DestroyActor(projectile);
		Destroys.Add(projectile);
	}

	for (ATestProjectile01* projectile : Destroys)
	{
		int removed = DestoryProjectiles.Remove(projectile);
	}
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

	PlayerInputComponent->BindKey(EKeys::W, EInputEvent::IE_Pressed, this, &ATestUE4_01Character::KeyWPressedStart);
	PlayerInputComponent->BindKey(EKeys::W, EInputEvent::IE_Released, this, &ATestUE4_01Character::KeyWPressedEnd);

	PlayerInputComponent->BindKey(EKeys::A, EInputEvent::IE_Pressed, this, &ATestUE4_01Character::KeyAPressedStart);
	PlayerInputComponent->BindKey(EKeys::A, EInputEvent::IE_Released, this, &ATestUE4_01Character::KeyAPressedEnd);
}

ATestProjectile01* ATestUE4_01Character::SpawnProjectileByActor(float InScale, float InAngle, bool bIsHitReflect, float InTimeDestroy, FColor InColor)
{
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GWorld, 0);
	UPawnMovementComponent* MoveComp = Character->GetMovementComponent();
	ensure(nullptr != MoveComp);

	FTransform TM = Character->GetTransform();
	FQuat Rotation = TM.GetRotation();
	FVector PosRelative = MoveComp->GetActorFeetLocation() - TM.GetLocation() + (Rotation.GetUpVector() * 50) + (Rotation.GetForwardVector() * 20);
	ATestProjectile01* Spawned = SpawnProjectile(TM, PosRelative, InScale, InAngle, bIsHitReflect, InTimeDestroy, InColor);
	return Spawned;
}

ATestProjectile01* ATestUE4_01Character::SpawnProjectile(const FTransform& InTM, const FVector& InPosRelative, float InScale, float InAngle, bool bIsHitReflect, float InTimeDestroy, FColor InColor)
{
	FQuat Rotation = InTM.GetRotation();
	FVector Dir = Rotation.GetForwardVector();
	FVector Up = Rotation.GetUpVector();
	FVector Location = InTM.GetLocation() + InPosRelative;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ATestProjectile01* NewProjectile = GWorld->SpawnActor<ATestProjectile01>(ATestProjectile01::StaticClass(), SpawnParams);
	if (FMath::Abs(InAngle) > KINDA_SMALL_NUMBER)
	{
		FRotator Rotator = Rotation.Rotator();
		Rotator.Pitch += InAngle;
		Dir = Rotator.Vector();
	}
	NewProjectile->InitProjectile(Location, Dir, InScale, bIsHitReflect, InTimeDestroy, InColor);
	return NewProjectile;
}

void ATestUE4_01Character::StartSkill_1()
{
	GWarn->Logf(ELogVerbosity::Display, TEXT("StartSkill_1"));
	SpawnProjectileByActor(1.0f);
}

void ATestUE4_01Character::StartSkill_2()
{
	GWarn->Logf(ELogVerbosity::Display, TEXT("StartSkill_2"));
	SpawnProjectileByActor(3.0f, 0.0f, false, 5.0f);
}

void ATestUE4_01Character::StartSkill_3()
{
	GWarn->Logf(ELogVerbosity::Display, TEXT("StartSkill_3"));
	TFunction<void(ATestProjectile01*, float)> OnTimeTrigger = [&](ATestProjectile01* projectile, float elapsed)
	{
		if (elapsed < 2.0f) return;

		FTransform tm = projectile->GetTransform();
		FVector relative = FVector::ZeroVector;
		SpawnProjectile(tm, relative, 1.0f, 0.0f);
		SpawnProjectile(tm, relative, 1.0f, 45.0f);
		SpawnProjectile(tm, relative, 1.0f, -45.0f);

		ATestUE4_01Character::AddDestoryRequest(projectile);
		projectile->ArrowComponent->bHiddenInGame = true;
	};
	ATestProjectile01* Spawned = nullptr;
	Spawned = SpawnProjectileByActor(1.0f, 0.0f, false, 4.0f);
	Spawned->OnTimeTrigger = OnTimeTrigger;

	Spawned = SpawnProjectileByActor(1.0f, 45.0f, false, 4.0f);
	Spawned->OnTimeTrigger = OnTimeTrigger;

	Spawned = SpawnProjectileByActor(1.0f, -45.0f, false, 4.0f);
	Spawned->OnTimeTrigger = OnTimeTrigger;
}

void ATestUE4_01Character::StartSkill_4()
{
	GWarn->Logf(ELogVerbosity::Display, TEXT("StartSkill_4"));
	SpawnProjectileByActor(1.0f, 0.0f, true, 5.0f, FColor(0, 0, 255));
}

void ATestUE4_01Character::StartSkill_5()
{
	GWarn->Logf(ELogVerbosity::Display, TEXT("StartSkill_5"));
	for (int i = 0; i < 360; i += 30)
	{
		SpawnProjectileByActor(0.5f, i, true, DEFAULT_DESTROY, FColor(0, 0, 255));
	}
}

void ATestUE4_01Character::MoveRight(float Value)
{
	// add movement in that direction
	AddMovementInput(FVector(0.f,-1.f,0.f), Value);
}

void ATestUE4_01Character::KeyQPressedStart()
{
	TestKeys[ETestKey::Q].StartPress();
}

void ATestUE4_01Character::KeyQPressedEnd()
{
	TestKeys[ETestKey::Q].EndPress();
}

void ATestUE4_01Character::KeyWPressedStart()
{
	TestKeys[ETestKey::W].StartPress();
}

void ATestUE4_01Character::KeyWPressedEnd()
{
	TestKeys[ETestKey::W].EndPress();
}

void ATestUE4_01Character::KeyAPressedStart()
{
	TestKeys[ETestKey::A].StartPress();
}

void ATestUE4_01Character::KeyAPressedEnd()
{
	TestKeys[ETestKey::A].EndPress();
}

void ATestUE4_01Character::AddDestoryRequest(ATestProjectile01* Projectile)
{
	DestoryProjectiles.Add(Projectile);
}

int ATestUE4_01Character::GetProgressSkill_2()
{
	return TestKeyActions[ESkill::_2].GetProgress() * 100.0f;
}
