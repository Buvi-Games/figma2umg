// Fill out your copyright notice in the Description page of Project Settings.

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

	UPROPERTY()
	TObjectPtr<UTextBlock> TextBlock = nullptr;
};
