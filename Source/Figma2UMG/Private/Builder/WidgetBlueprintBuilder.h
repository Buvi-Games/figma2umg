// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SwitcherBuilder.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "Components/WidgetSwitcher.h"
#include "Parser/Properties/FigmaComponentProperty.h"

class UK2Node_FunctionEntry;
class UK2Node_FunctionResult;
class UK2Node_VariableGet;
class UK2Node_IfThenElse;
class UWidgetBlueprint;

class WidgetBlueprintBuilder
{
public:
	static FIGMA2UMG_API void PatchVisibilityBind(TObjectPtr<UWidgetBlueprint> WidgetBP, TObjectPtr<UWidget> Widget, const FBPVariableDescription& VariableDescription, const FName& VariableName);
	static FIGMA2UMG_API void PatchTextBind(TObjectPtr<UWidgetBlueprint> WidgetBP, TObjectPtr<UTextBlock> TextBlock, const FName& VariableName);
	static FIGMA2UMG_API void CreateSwitchFunction(TObjectPtr<UWidgetBlueprint> WidgetBP, const FString& PropertyName, const FBPVariableDescription* VariableMap);
	static FIGMA2UMG_API void PatchSwitchFunction(TObjectPtr<UWidgetBlueprint> WidgetBP, TObjectPtr<UWidgetSwitcher> WidgetSwitcher, const FString& PropertyName);
	static FIGMA2UMG_API void SetPropertyValue(TObjectPtr<UUserWidget> Widget, const FName& VariableName, const FFigmaComponentProperty& ComponentProperty);

private:
	static FIGMA2UMG_API void AddBindingFunction(TObjectPtr<UWidgetBlueprint> WidgetBP, TObjectPtr<UWidget> Widget, UEdGraph* FunctionGraph, const FName& PropertyName);
	static FIGMA2UMG_API void AddBindingProperty(TObjectPtr<UWidgetBlueprint> WidgetBP, TObjectPtr<UWidget> Widget, const FName& PropertyName, const FName& MemberPropertyName);

	static FIGMA2UMG_API UK2Node_FunctionEntry* PatchFunctionEntry(UEdGraph* Graph, const FString VarName, FName VarType, EPinContainerType VarContainerType, const FString& DefaultValue);
	static FIGMA2UMG_API UK2Node_VariableGet* PatchVariableGetNode(TObjectPtr<UWidgetBlueprint> WidgetBP, UEdGraph* Graph, FName VariableName, FVector2D NodeLocation);
	static FIGMA2UMG_API UK2Node_IfThenElse* PatchIfThenElseNode(UEdGraph* Graph, FVector2D NodeLocation, UEdGraphPin* ExecPin, UEdGraphPin* ConditionValuePin, UEdGraphPin* ThenReturnPin, UEdGraphPin* ElseReturnPin);
	static FIGMA2UMG_API UK2Node_FunctionResult* PatchFunctionResult(UEdGraph* Graph, FVector2D NodeLocation, const FString& ReturnValue);

};
