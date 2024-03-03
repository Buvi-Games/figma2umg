// Fill out your copyright notice in the Description page of Project Settings.


#include "Builder/SwitcherBuilder.h"

#include "WidgetBlueprintBuilder.h"
#include "Blueprint/WidgetTree.h"
#include "Components/WidgetSwitcher.h"

TObjectPtr<UWidgetSwitcher> FSwitcherBuilder::Patch(TObjectPtr<UWidget> WidgetToPatch, UObject* AssetOuter)
{
	WidgetSwitcher = WidgetToPatch ? Cast<UWidgetSwitcher>(WidgetToPatch) : nullptr;

	WidgetBlueprintBuilder::PatchSwitchFunction(Cast<UWidgetBlueprint>(AssetOuter->GetOuter()), WidgetSwitcher, PropertyName, PropertyDefinition.VariantOptions);

	return WidgetSwitcher;
}

void FSwitcherBuilder::AddVariation(UWidgetBlueprint* WidgetBP)
{
	// The UWidgetSwitcher needs to be created during the creation of the UAsset so it can be compiled and visible during patch.
	TArray<UWidget*> Widgets;
	WidgetBP->WidgetTree->GetAllWidgets(Widgets);
	FString NameToFind = PropertyName;
	UWidget** Found = Widgets.FindByPredicate([NameToFind](const UWidget* Widget) {return Widget->IsA<UWidgetSwitcher>() && Widget->GetName() == NameToFind; });
	WidgetSwitcher = Found ? Cast<UWidgetSwitcher>(*Found) : nullptr;
	if (WidgetSwitcher == nullptr)
	{
		WidgetSwitcher = NewObject<UWidgetSwitcher>(WidgetBP->WidgetTree, *PropertyName);
		if (Widgets.IsEmpty())
		{
			WidgetBP->WidgetTree->RootWidget = WidgetSwitcher;
		}
		else
		{
			UWidgetSwitcher* ParentSwitch = Cast<UWidgetSwitcher>(Widgets.Last());
			ParentSwitch->AddChild(WidgetSwitcher);
		}
	}
	else if (WidgetSwitcher->GetName() != PropertyName)
	{
		WidgetSwitcher->Rename(*PropertyName);
	}

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
