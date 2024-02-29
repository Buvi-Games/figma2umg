// Fill out your copyright notice in the Description page of Project Settings.


#include "Builder/SwitcherBuilder.h"

#include "WidgetBlueprintBuilder.h"
#include "Components/WidgetSwitcher.h"

TObjectPtr<UWidgetSwitcher> FSwitcherBuilder::Patch(TObjectPtr<UWidget> WidgetToPatch, UObject* AssetOuter)
{
	WidgetSwitcher = WidgetToPatch ? Cast<UWidgetSwitcher>(WidgetToPatch) : nullptr;
	if (WidgetSwitcher == nullptr)
	{
		WidgetToPatch = WidgetSwitcher = NewObject<UWidgetSwitcher>(AssetOuter, *PropertyName);
	}
	else if (WidgetSwitcher->GetName() != PropertyName)
	{
		WidgetSwitcher->Rename(*PropertyName);
	}

	WidgetBlueprintBuilder::PatchSwitchFunction(Cast<UWidgetBlueprint>(AssetOuter->GetOuter()), WidgetSwitcher, PropertyName, PropertyDefinition.VariantOptions);

	return WidgetSwitcher;
}

void FSwitcherBuilder::AddVariation(UWidgetBlueprint* WidgetBP)
{
	WidgetBlueprintBuilder::CreateSwitchFunction(WidgetBP, PropertyName, PropertyDefinition.VariantOptions);
}

void FSwitcherBuilder::Reset()
{
	WidgetSwitcher = nullptr;
}

void FSwitcherBuilder::SetProperty(const FString& InPropertyName, const FFigmaComponentPropertyDefinition& InDefinition)
{
	PropertyName = InPropertyName;
	PropertyDefinition = InDefinition;
}
