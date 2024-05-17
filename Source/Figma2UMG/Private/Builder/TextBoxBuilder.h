// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "TextBoxBuilder.generated.h"

struct FFigmaTypeStyle;
struct FFigmaPaint;
class UTextBlock;

USTRUCT()
struct FIGMA2UMG_API FTextBoxBuilder
{
public:
	GENERATED_BODY()

	void SetStyle(const FFigmaTypeStyle& Style);
	void SetFill(const FFigmaPaint& Fill);
	void Reset();

	float ConvertFontSizeFromDisplayToNative(float DisplayFontSize) const;
	void SetupWidget(TObjectPtr<UWidget> Widget);

	UPROPERTY()
	TObjectPtr<UTextBlock> TextBlock = nullptr;
};
