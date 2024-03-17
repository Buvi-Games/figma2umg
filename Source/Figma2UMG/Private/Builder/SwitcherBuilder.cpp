// Fill out your copyright notice in the Description page of Project Settings.


#include "Builder/SwitcherBuilder.h"

#include "Figma2UMGModule.h"
#include "WidgetBlueprintBuilder.h"
#include "Blueprint/WidgetTree.h"
#include "Components/WidgetSwitcher.h"
#include "Interfaces/WidgetOwner.h"

TObjectPtr<UWidgetSwitcher> FSwitcherBuilder::Patch(TObjectPtr<UWidget> WidgetToPatch, UObject* AssetOuter)
{
	WidgetSwitcher = WidgetToPatch ? Cast<UWidgetSwitcher>(WidgetToPatch) : nullptr;

	WidgetBlueprintBuilder::PatchSwitchFunction(Cast<UWidgetBlueprint>(AssetOuter->GetOuter()), WidgetSwitcher, PropertyName, PropertyDefinition.VariantOptions);

	return WidgetSwitcher;
}

void FSwitcherBuilder::SetupWidget(TObjectPtr<UWidget> Widget)
{
	WidgetSwitcher = Cast<UWidgetSwitcher>(Widget);
	if (!WidgetSwitcher)
	{
		if (Widget)
		{
			UE_LOG_Figma2UMG(Error, TEXT("[FSwitcherBuilder::SetupWidget] Fail to setup UPanelWidget from UWidget %s of type %s."), *Widget->GetName(), *Widget->GetClass()->GetDisplayNameText().ToString());
		}
		else
		{
			UE_LOG_Figma2UMG(Warning, TEXT("[FSwitcherBuilder::SetupWidget] Fail to setup UPanelWidget from a null Widget."));
		}
	}
}

void FSwitcherBuilder::FindAndSetWidget(const TArray<UWidget*>& Widgets)
{
	FString NameToFind = PropertyName;
	UWidget* const* Found = Widgets.FindByPredicate([NameToFind](const UWidget* Widget) {return Widget->IsA<UWidgetSwitcher>() && Widget->GetName() == NameToFind; });
	SetupWidget(Found ? (*Found) : nullptr);
}

void FSwitcherBuilder::AddVariation(UWidgetBlueprint* WidgetBP)
{
	// The UWidgetSwitcher needs to be created during the creation of the UAsset so it can be compiled and visible during patch.
	TArray<UWidget*> Widgets;
	WidgetBP->WidgetTree->GetAllWidgets(Widgets);
	FString NameToFind = PropertyName;
	TArray<UWidgetSwitcher*> SwitchWidgets;
	for(UWidget* Widget: Widgets)
	{
		if(Widget->IsA<UWidgetSwitcher>())
		{
			SwitchWidgets.Add(Cast<UWidgetSwitcher>(Widget));
		}
	}


	UWidgetSwitcher** Found = SwitchWidgets.FindByPredicate([NameToFind](const UWidget* Widget) {return Widget->IsA<UWidgetSwitcher>() && Widget->GetName() == NameToFind; });
	WidgetSwitcher = Found ? Cast<UWidgetSwitcher>(*Found) : nullptr;
	if (WidgetSwitcher == nullptr)
	{
		WidgetSwitcher = IWidgetOwner::NewWidget<UWidgetSwitcher>(WidgetBP->WidgetTree, *PropertyName);
		if (SwitchWidgets.IsEmpty())
		{
			WidgetBP->WidgetTree->RootWidget = WidgetSwitcher;
		}
		else
		{
			UWidgetSwitcher* ParentSwitch = Cast<UWidgetSwitcher>(SwitchWidgets.Last());
			ParentSwitch->AddChild(WidgetSwitcher);
		}
	}
	else if (WidgetSwitcher->GetName() != PropertyName)
	{
		IWidgetOwner::TryRenameWidget(PropertyName, WidgetSwitcher);
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
