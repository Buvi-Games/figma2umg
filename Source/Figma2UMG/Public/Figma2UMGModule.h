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

	UFigma2UMGSettings* ModuleSettings;
};
