// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestUE4_01GameMode.h"
#include "TestUE4_01Character.h"
#include "UObject/ConstructorHelpers.h"

ATestUE4_01GameMode::ATestUE4_01GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/SideScrollerCPP/Blueprints/SideScrollerCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ATestUE4_01GameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	GWarn->Logf(ELogVerbosity::Display, TEXT("ATestUE4_01GameMode::InitGame"));
}

void ATestUE4_01GameMode::InitGameState()
{
	Super::InitGameState();
	GWarn->Logf(ELogVerbosity::Display, TEXT("ATestUE4_01GameMode::InitGameState"));
}
