// Copyright 2024 Buvi Games. All Rights Reserved.
#pragma once
#include "Templates/UniquePtr.h"

class FSlateStyleSet;
class ISlateStyle;
class FFigma2UMGStyle
{
public:
	static void Initialize();
	static void Shutdown();

	static const ISlateStyle& Get();
	static FName GetStyleSetName();
	static FName GetContextName();

	static void SetIcon(const FString& StyleName, const FString& ResourcePath);
	static void SetSVGIcon(const FString& StyleName, const FString& ResourcePath);

private:
	static TUniquePtr<FSlateStyleSet> Create();
	static TUniquePtr<FSlateStyleSet> MSStyleInstance;
};
