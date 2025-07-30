// MIT License
// Copyright (c) 2024 Buvi Games

#pragma once

#include "CoreMinimal.h"

class FIGMA2UMG_API FFigma2UMGManager : public TSharedFromThis<FFigma2UMGManager>
{
public:
	FFigma2UMGManager();
	virtual ~FFigma2UMGManager();

	void Initialize();
	void Shutdown();

private:
	void SetupMenuItem();
	void CreateWindow();

	TSharedRef<SDockTab> CreateTab(const FSpawnTabArgs& Args);

	const FText TabDisplay = FText::FromString("Figma2UMG");
	const FText ToolTip = FText::FromString("Launch Figma2UMG Importer");

	TSharedPtr<SDockTab> ImporterDockTab;
};
