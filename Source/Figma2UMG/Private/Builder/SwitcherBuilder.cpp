// Fill out your copyright notice in the Description page of Project Settings.


#include "Builder/SwitcherBuilder.h"

#include "WidgetBlueprintBuilder.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet2/BlueprintEditorUtils.h"

TObjectPtr<UWidgetSwitcher> FSwitcherBuilder::Patch(TObjectPtr<UWidget> WidgetToPatch, UObject* AssetOuter, const FString& WidgetName)
{
	UWidgetSwitcher* MainWidget = WidgetToPatch ? Cast<UWidgetSwitcher>(WidgetToPatch) : nullptr;
	if (MainWidget == nullptr)
	{
		WidgetToPatch = MainWidget = NewObject<UWidgetSwitcher>(AssetOuter, *WidgetName);
	}
	else if (MainWidget->GetName() != WidgetName)
	{
		MainWidget->Rename(*WidgetName);
	}

	WidgetBlueprintBuilder::PatchSwitchFunction(Cast<UWidgetBlueprint>(AssetOuter->GetOuter()), MainWidget, FunctionName);

	return WidgetSwitcher;
}

void FSwitcherBuilder::AddVariation(UWidgetBlueprint* WidgetBP, const FString& PropertyName, const FFigmaComponentPropertyDefinition& PropertyDefinition)
{
	FEdGraphPinType MemberType;
	MemberType.PinCategory = UEdGraphSchema_K2::PC_String;
	MemberType.ContainerType = EPinContainerType::Map;

	FString PropertyMapName = PropertyName + "Map";//TODO: Remove '#id'
	FBlueprintEditorUtils::AddMemberVariable(WidgetBP, *PropertyMapName, MemberType, PropertyDefinition.DefaultValue);
	
	//FBlueprintEditorUtils::SetBlueprintOnlyEditableFlag(WidgetBP, *PropertyMapName, false);
	FBPVariableDescription* NewVariable = WidgetBP->NewVariables.FindByPredicate([PropertyMapName](const FBPVariableDescription& NewVariable) {return NewVariable.VarName == PropertyMapName; });
	if (NewVariable)
	{
		WidgetBlueprintBuilder::CreateSwitchFunction(WidgetBP, PropertyName, NewVariable);
		FunctionName = PropertyName;
	}	
}

void FSwitcherBuilder::Reset()
{
	WidgetSwitcher = nullptr;
}
