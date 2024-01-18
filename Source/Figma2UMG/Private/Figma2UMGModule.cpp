// Copyright Epic Games, Inc. All Rights Reserved.

#include "Figma2UMGModule.h"

#include "UI/Figma2UMGManager.h"
#include "UI/Figma2UMGStyle.h"

#define LOCTEXT_NAMESPACE "Figma2UMGModule"

TSharedPtr<FFigma2UMGManager> FFigma2UMGModule::Instance;

void FFigma2UMGModule::StartupModule()
{
	if (!Instance.IsValid())
	{
		Instance = MakeShareable(new FFigma2UMGManager);
		Instance->Initialize();
	}
}

void FFigma2UMGModule::ShutdownModule()
{
	if (Instance.IsValid())
	{
		Instance->Shutdown();
		Instance = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FFigma2UMGModule, FFigma2UMG)