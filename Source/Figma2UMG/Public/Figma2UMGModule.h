// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class UFigma2UMGSettings;
class FFigma2UMGManager;

class FFigma2UMGModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	UFigma2UMGSettings* GetSettings() const;

private:
	static TSharedPtr<FFigma2UMGManager> Instance;

	UFigma2UMGSettings* ModuleSettings = nullptr;
};

FIGMA2UMG_API DECLARE_LOG_CATEGORY_EXTERN(LogFigma2UMG, Log, All);

#define UE_LOG_Figma2UMG(Verbosity, Format, ...)																\
{																												\
	UE_LOG(LogFigma2UMG, Verbosity, Format, ##__VA_ARGS__);														\
}

#define UE_CLOG_Figma2UMG(Conditional, Verbosity, Format, ...)																\
{																															\
	UE_CLOG(Conditional, LogFigma2UMG, Verbosity, Format, ##__VA_ARGS__);													\
}