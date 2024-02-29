// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBlueprint.h"
#include "Parser/Properties/FigmaComponentPropertyDefinition.h"

#include "SwitcherBuilder.generated.h"

class UWidget;
class UWidgetSwitcher;

USTRUCT()
struct FIGMA2UMG_API FSwitcherBuilder
{
public:
	GENERATED_BODY()
	virtual ~FSwitcherBuilder() = default;

	TObjectPtr<UWidgetSwitcher> Patch(TObjectPtr<UWidget> WidgetToPatch, UObject* AssetOuter, const FString& WidgetName);
	void AddVariation(UWidgetBlueprint* WidgetBP, const FString& Key, const FFigmaComponentPropertyDefinition& Value);

	void Reset();
private:
	UPROPERTY()
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher = nullptr;

	UPROPERTY()
	FString FunctionName;
};
