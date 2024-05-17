// Copyright 2024 Buvi Games. All Rights Reserved.

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

	TObjectPtr<UWidgetSwitcher> Patch(TObjectPtr<UWidget> WidgetToPatch, UObject* AssetOuter);
	void SetupWidget(TObjectPtr<UWidget> Widget);
	void FindAndSetWidget(const TArray<UWidget*>& Widgets);
	void AddVariation(UWidgetBlueprint* WidgetBP);

	void Reset();
	void SetProperty(const FString& InPropertyName, const FFigmaComponentPropertyDefinition& InDefinition);

	TObjectPtr<UWidgetSwitcher> GetWidgetSwitcher() const { return WidgetSwitcher; }
	FString GetPropertyName() const {return PropertyName;}

private:
	UPROPERTY()
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher = nullptr;

	FString PropertyName;
	FFigmaComponentPropertyDefinition PropertyDefinition;

};
