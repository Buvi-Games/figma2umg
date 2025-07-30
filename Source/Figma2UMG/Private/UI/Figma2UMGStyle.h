// MIT License
// Copyright (c) 2024 Buvi Games
#pragma once
#include "Templates/UniquePtr.h"

class FSlateStyleSet;
class ISlateStyle;
class FIGMA2UMG_API FFigma2UMGStyle
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
