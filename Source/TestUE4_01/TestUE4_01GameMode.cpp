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
