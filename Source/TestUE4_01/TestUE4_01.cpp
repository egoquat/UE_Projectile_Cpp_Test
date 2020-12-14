// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestUE4_01.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, TestUE4_01, "TestUE4_01" );

void FTestUE4_01Module::StartupModule()
{

}

void FTestUE4_01Module::ShutdownModule()
{

}

void UTestUE4_01::Init(class IEngineLoop* InEngineLoop)
{
	Super::Init(InEngineLoop);
	GWarn->Logf(ELogVerbosity::Display, TEXT("UTestUE4_01::Init"));
}

void UTestUE4_01::Start()
{
	Super::Start();
	GWarn->Logf(ELogVerbosity::Display, TEXT("UTestUE4_01::Start"));

}

void UTestUE4_01::PreExit()
{
	Super::PreExit();
	GWarn->Logf(ELogVerbosity::Display, TEXT("UTestUE4_01::PreExit"));

}

void UTestUE4_01::Tick(float DeltaSeconds, bool bIdleMode)
{

}