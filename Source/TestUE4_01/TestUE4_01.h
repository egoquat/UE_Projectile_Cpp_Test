// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "TestUE4_01.generated.h"

class FTestUE4_01Module : public IModuleInterface
{
public:

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

UCLASS()
class UTestUE4_01 : public UGameEngine
{
	GENERATED_BODY()

private:
	virtual void	Init(class IEngineLoop* InEngineLoop) override;
	virtual void	Start() override;

	virtual void	PreExit() override;
	virtual void	Tick(float DeltaSeconds, bool bIdleMode) override;
};