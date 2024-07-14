// Copyright 2024 Buvi Games. All Rights Reserved.

#include "Figma2UMGModule.h"

#include "Settings/Figma2UMGSettings.h"
#include "ISettingsModule.h"
#include "UI/Figma2UMGManager.h"

#define LOCTEXT_NAMESPACE "Figma2UMGModule"

TSharedPtr<FFigma2UMGManager> FFigma2UMGModule::Instance;

void FFigma2UMGModule::StartupModule()
{
	ModuleSettings = NewObject<UFigma2UMGSettings>(GetTransientPackage(), "Figma2UMGSettings", RF_Standalone);
	ModuleSettings->AddToRoot();

	// Register settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "Figma2UMG",
			LOCTEXT("RuntimeSettingsName", "Figma2UMG"),
			LOCTEXT("RuntimeSettingsDescription", "Configure Figma2UMG plugin settings"),
			ModuleSettings);
	}

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

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "VaRest");
	}

	if (!GExitPurge)
	{
		ModuleSettings->RemoveFromRoot();
	}
	else
	{
		ModuleSettings = nullptr;
	}
}

UFigma2UMGSettings* FFigma2UMGModule::GetSettings() const
{
	check(ModuleSettings);
	return ModuleSettings;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FFigma2UMGModule, FFigma2UMG)
DEFINE_LOG_CATEGORY(LogFigma2UMG)
