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

#include "TestProjectile.h"
#include "TestProjectileSpline.h"
#include "TestUE4_01UI.h"

TSet<ATestProjectile*> ATestUE4_01Character::DestroyProjectiles;
TArray<AActor*> ATestUE4_01Character::FirstActors;

static TArray<FTestKeyEvent> TestKeys;
static TArray<FKeyAction> TestKeyActions;
static TQueue<TFunction<void()>> QueueOnSpawns;

//////////////////////////////////////////////////////////////////////////
// Settings

void FTestKeyEvent::Reset()
{
	IsEvented = IsReleased = false;
	TimePressRelease = 0.0f;
}

float FTestKeyEvent::GetElapsePressed() { return FPlatformTime::Seconds() - TimePressStart; }

void FTestKeyEvent::StartPress()
{
	Reset();
	if (true == IsPressed)
		return;
	IsPressed = true;
	TimePressStart = FPlatformTime::Seconds();
	GWarn->Logf(ELogVerbosity::Display, TEXT(" Start PressKey/%d/Time:%f"), Key, TimePressStart);
	GFrameCounter;
}

float FTestKeyEvent::EndPress()
{
	if (false == IsPressed) return -1.0f;
	IsPressed = false;
	float TimeCurrent = FPlatformTime::Seconds();
	TimePressRelease = TimeCurrent - TimePressStart;
	GWarn->Logf(ELogVerbosity::Display, TEXT(" End PressKey/%d/Time:%f/Elapsed:%f"), Key, TimeCurrent, TimePressRelease);
	IsReleased = true;
	return TimePressRelease;
}

void FTestKeyEvent::TickPost()
{
	Reset();
}

float FKeyAction::GetProgress()
{
	if (nullptr == OnProgress) return 1.0f;
	return OnProgress();
}

FKeyAction::FKeyAction(ESkill typeSkill, TFunction<void()> action, TFunction<bool(float)> cond, TFunction<float()> onprogress)
{
	TypeSkill = typeSkill;
	Action = action;
	Cond = cond;
	OnProgress = onprogress;
}

void FKeyAction::TickAction(float deltaSec)
{
	if (false == Cond(deltaSec))
		return;

	float timeCurrent = FPlatformTime::Seconds();
	if (timeCurrent - TimeLastCall < DEFAULT_ACTION_DELTA)
		return;

	Action();
	TimeLastCall = timeCurrent;
}

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
}

void ATestUE4_01Character::BeginPlay()
{
	// Init action skill
	if (TestKeys.Num() <= 0)
	{
		TestKeys.Add(FTestKeyEvent(ETestKey::Q));
		TestKeys.Add(FTestKeyEvent(ETestKey::W));
		TestKeys.Add(FTestKeyEvent(ETestKey::E));
		TestKeys.Add(FTestKeyEvent(ETestKey::R));
		TestKeys.Add(FTestKeyEvent(ETestKey::A));

		TFunction<bool(float)> condition = nullptr;
		TFunction<float()> onprogress = nullptr;
		condition = [&](float deltaSec)
		{
			FTestKeyEvent& keyQ = TestKeys[(int)ETestKey::Q];
			return (true == keyQ.IsReleased && 3.0f > keyQ.TimePressRelease);
		};
		TestKeyActions.Add(FKeyAction(ESkill::Skill01, [&]() {StartSkill_1(); }, condition));

		condition = [&](float deltaSec)
		{
			FTestKeyEvent& keyQ = TestKeys[(int)ETestKey::Q];
			return (true == keyQ.IsReleased && 3.0f <= keyQ.TimePressRelease);
		};
		onprogress = [&]()
		{
			FTestKeyEvent& keyQ = TestKeys[(int)ETestKey::Q];
			float progress = 0.0f;
			if (true == keyQ.IsPressed && keyQ.GetElapsePressed() > 0.0f)
			{
				progress = (keyQ.GetElapsePressed()) / (3.0f);
				progress = FMath::Clamp(progress, 0.0f, 1.0f);
				//GWarn->Logf(ELogVerbosity::Display, TEXT(" progress Press:%f/ Elapsed:%f"), progress, keyQ.GetElapsePressed());
			}
			return progress;
		};
		TestKeyActions.Add(FKeyAction(ESkill::Skill02, [&]() {StartSkill_2(); }, condition, onprogress));

		condition = [&](float deltaSec)
		{
			FTestKeyEvent& keyQ = TestKeys[(int)ETestKey::Q];
			FTestKeyEvent& keyW = TestKeys[(int)ETestKey::W];
			return (true == keyQ.IsPressed && 1.0f >= keyQ.GetElapsePressed() && true == keyW.IsPressed);
		};
		TestKeyActions.Add(FKeyAction(ESkill::Skill03, [&]() {StartSkill_3(); }, condition));

		condition = [&](float deltaSec)
		{
			FTestKeyEvent& keyQ = TestKeys[(int)ETestKey::Q];
			FTestKeyEvent& keyW = TestKeys[(int)ETestKey::W];
			return (false == keyQ.IsPressed && true == keyW.IsReleased);
		};
		TestKeyActions.Add(FKeyAction(ESkill::Skill04, [&]() {StartSkill_4(); }, condition));

		condition = [&](float deltaSec)
		{
			FTestKeyEvent& keyA = TestKeys[(int)ETestKey::A];
			return (true == keyA.IsReleased);
		};
		TestKeyActions.Add(FKeyAction(ESkill::Skill05, [&]() {StartSkill_5(); }, condition));

		condition = [&](float deltaSec)
		{
			FTestKeyEvent& keyE = TestKeys[(int)ETestKey::E];
			return (true == keyE.IsReleased);
		};
		TestKeyActions.Add(FKeyAction(ESkill::Skill06, [&]() {StartSkill_6(); }, condition));
	}

	FTestUE4_01UI::InitTestUI();

	Super::BeginPlay();
	FirstActors.Add(this);

	APlayerController* PlayerController = GEngine->GetFirstLocalPlayerController(GWorld);
	if (nullptr != PlayerController)
	{
		APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
		CameraManager->SetFOV(90);
	}

	USkeletalMeshComponent* Component = Cast<USkeletalMeshComponent>(GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	this->WasRecentlyRendered(0.025f);
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

	TSet<ATestProjectile*> Destroys;
	for (ATestProjectile* projectile : DestroyProjectiles)
	{
		GWorld->DestroyActor(projectile);
		Destroys.Add(projectile);
	}

	for (ATestProjectile* projectile : Destroys)
	{
		int removed = DestroyProjectiles.Remove(projectile);
	}

	int CountIter = 0;
	while(CountIter++ < DEFAULT_DEQUEUE_PERFRAME && false == QueueOnSpawns.IsEmpty())
	{
		TFunction<void()> OnSpawn = nullptr;
		QueueOnSpawns.Dequeue(OnSpawn);
		OnSpawn();
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

	PlayerInputComponent->BindKey(EKeys::E, EInputEvent::IE_Pressed, this, &ATestUE4_01Character::KeyEPressedStart);
	PlayerInputComponent->BindKey(EKeys::E, EInputEvent::IE_Released, this, &ATestUE4_01Character::KeyEPressedEnd);

	PlayerInputComponent->BindKey(EKeys::A, EInputEvent::IE_Pressed, this, &ATestUE4_01Character::KeyAPressedStart);
	PlayerInputComponent->BindKey(EKeys::A, EInputEvent::IE_Released, this, &ATestUE4_01Character::KeyAPressedEnd);
}

template<class T>
T* ATestUE4_01Character::SpawnProjectileByActor(float InScale, float InAngle, bool bIsHitReflect, float InTimeDestroy, FColor InColor)
{
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GWorld, 0);
	UPawnMovementComponent* MoveComp = Character->GetMovementComponent();
	ensure(nullptr != MoveComp);

	FTransform TM = Character->GetTransform();
	FQuat Rotation = TM.GetRotation();
	FVector PosRelative = MoveComp->GetActorFeetLocation() - TM.GetLocation() + (Rotation.GetUpVector() * 50) + (Rotation.GetForwardVector() * 20);
	T* Spawned = SpawnProjectile<T>(TM, PosRelative, InScale, InAngle, bIsHitReflect, InTimeDestroy, InColor);
	return Spawned;
}

template<class T>
void ATestUE4_01Character::SpawnProjectileQueued(const FTransform& InTM, const FVector& InPosRelative, float InScale, float InAngle, bool bIsHitReflect, float InTimeDestroy, FColor InColor)
{
	TFunction<void()> OnSpawn = [=]()
	{
		SpawnProjectile<T>(InTM, InPosRelative, InScale, InAngle, bIsHitReflect, InTimeDestroy, InColor);
	};
	QueueOnSpawns.Enqueue(OnSpawn);
}

template<class T>
T* ATestUE4_01Character::SpawnProjectile(const FTransform& InTM, const FVector& InPosRelative, float InScale, float InAngle, bool bIsHitReflect, float InTimeDestroy, FColor InColor)
{
	FQuat Rotation = InTM.GetRotation();
	FVector Dir = Rotation.GetForwardVector();
	FVector Up = Rotation.GetUpVector();
	FVector Location = InTM.GetLocation() + InPosRelative;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	T* NewProjectile = GWorld->SpawnActor<T>(T::StaticClass(), SpawnParams);
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
	SpawnProjectileByActor<ATestProjectile>(1.0f);
}

void ATestUE4_01Character::StartSkill_2()
{
	GWarn->Logf(ELogVerbosity::Display, TEXT("StartSkill_2"));
	SpawnProjectileByActor<ATestProjectile>(3.0f, 0.0f, false, 5.0f);
}

void ATestUE4_01Character::StartSkill_3()
{
	GWarn->Logf(ELogVerbosity::Display, TEXT("StartSkill_3"));
	TFunction<void(ATestProjectile*, float)> OnTimeTrigger = [&](ATestProjectile* projectile, float elapsed)
	{
		if (elapsed < DEFAULT_TIME_TRIGGER) return;

		FTransform tm = projectile->GetTransform();
		FVector relative = FVector::ZeroVector;
		SpawnProjectileQueued<ATestProjectile>(tm, relative, 1.0f, 0.0f);
		SpawnProjectileQueued<ATestProjectile>(tm, relative, 1.0f, 45.0f);
		SpawnProjectileQueued<ATestProjectile>(tm, relative, 1.0f, -45.0f);

		ATestUE4_01Character::AddDestroyRequest(projectile);
		projectile->ArrowComponent->bHiddenInGame = true;
	};
	ATestProjectile* Spawned = nullptr;
	Spawned = SpawnProjectileByActor<ATestProjectile>(1.0f, 0.0f, false, 4.0f);
	Spawned->OnTimeTrigger = OnTimeTrigger;

	Spawned = SpawnProjectileByActor<ATestProjectile>(1.0f, 45.0f, false, 4.0f);
	Spawned->OnTimeTrigger = OnTimeTrigger;

	Spawned = SpawnProjectileByActor<ATestProjectile>(1.0f, -45.0f, false, 4.0f);
	Spawned->OnTimeTrigger = OnTimeTrigger;
}

void ATestUE4_01Character::StartSkill_4()
{
	GWarn->Logf(ELogVerbosity::Display, TEXT("StartSkill_4"));
	SpawnProjectileByActor<ATestProjectile>(1.0f, 0.0f, true, 5.0f, FColor(0, 0, 255));
}

void ATestUE4_01Character::StartSkill_5()
{
	GWarn->Logf(ELogVerbosity::Display, TEXT("StartSkill_5"));
	TFunction<void(ATestProjectile*, float)> OnTimeTrigger = [&](ATestProjectile* projectile, float elapsed)
	{
		if (elapsed < DEFAULT_TIME_TRIGGER - 1.0f) return;

		FTransform tm = projectile->GetTransform();
		FVector relative = FVector::ZeroVector;
		for (int i = 0; i < 360; i += 45)
		{
			SpawnProjectileQueued<ATestProjectile>(tm, relative, 0.5f, i, true, DEFAULT_DESTROY, FColor::Cyan);
		}

		ATestUE4_01Character::AddDestroyRequest(projectile);
		projectile->ArrowComponent->bHiddenInGame = true;
	};

	for (int i = 0; i < 360; i += 30)
	{
		ATestProjectile* Spawned = SpawnProjectileByActor<ATestProjectile>(0.5f, i, true, 4.0f, FColor(0, 0, 255));
		Spawned->OnTimeTrigger = OnTimeTrigger;
	}
}

void ATestUE4_01Character::StartSkill_6()
{
	GWarn->Logf(ELogVerbosity::Display, TEXT("StartSkill_6"));
	SpawnProjectileByActor<ATestProjectileSpline>(1.0f, 0.0f, true, 5.0f, FColor(0, 0, 255));
}

void ATestUE4_01Character::MoveRight(float Value)
{
	// add movement in that direction
	AddMovementInput(FVector(0.f,-1.f,0.f), Value);
}


void ATestUE4_01Character::KeyQPressedStart()
{
	TestKeys[(int)ETestKey::Q].StartPress();
}

void ATestUE4_01Character::KeyQPressedEnd()
{
	TestKeys[(int)ETestKey::Q].EndPress();
}

void ATestUE4_01Character::KeyWPressedStart()
{
	TestKeys[(int)ETestKey::W].StartPress();
}

void ATestUE4_01Character::KeyWPressedEnd()
{
	TestKeys[(int)ETestKey::W].EndPress();
}

void ATestUE4_01Character::KeyEPressedStart()
{
	TestKeys[(int)ETestKey::E].StartPress();
}

void ATestUE4_01Character::KeyEPressedEnd()
{
	TestKeys[(int)ETestKey::E].EndPress();
}

void ATestUE4_01Character::KeyAPressedStart()
{
	TestKeys[(int)ETestKey::A].StartPress();
}

void ATestUE4_01Character::KeyAPressedEnd()
{
	TestKeys[(int)ETestKey::A].EndPress();
}

void ATestUE4_01Character::KeyRPressedEnd()
{
	ATestProjectile::DestroyRequestAll();
}

void ATestUE4_01Character::AddDestroyRequest(ATestProjectile* Projectile)
{
	DestroyProjectiles.Add(Projectile);
}

int ATestUE4_01Character::GetProgressSkill_2()
{
	return TestKeyActions[(int)ESkill::Skill02].GetProgress() * 100.0f;
}
