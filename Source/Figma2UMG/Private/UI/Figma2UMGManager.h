// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FFigma2UMGManager : public TSharedFromThis<FFigma2UMGManager>
{
public:
	FFigma2UMGManager();
	virtual ~FFigma2UMGManager();

	void Initialize();
	void Shutdown();

private:
	void SetupMenuItem();
	void CreateWindow();
};
