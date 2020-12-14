// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TestUE4_01GameMode.generated.h"

UCLASS(minimalapi)
class ATestUE4_01GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATestUE4_01GameMode();

private:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() override;
};



