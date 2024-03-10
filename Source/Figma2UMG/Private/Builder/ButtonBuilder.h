// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Parser/Nodes/FigmaNode.h"
#include "Parser/Properties/FigmaComponentPropertyDefinition.h"

#include "ButtonBuilder.generated.h"

class UWidget;
class UButton;
class UFigmaComponent;

USTRUCT()
struct FIGMA2UMG_API FButtonBuilder
{
public:
	GENERATED_BODY()

	TObjectPtr<UWidget> Patch(TObjectPtr<UWidget> WidgetToPatch, UObject* AssetOuter);
	void SetupWidget(TObjectPtr<UWidget> Widget);
	void Reset();
	void SetProperty(const FString& InPropertyName, const FFigmaComponentPropertyDefinition& InDefinition);

	FString GetDefaultName() const;
	FString GetHoveredName() const;
	FString GetPressedName() const;
	FString GetDisabledName() const;
	FString GetFocusedName() const;

	TObjectPtr<UButton> GetWidget() const;

	void PatchStyle(const UFigmaComponent* DefaultComponent, const UFigmaComponent* HoveredComponent, const UFigmaComponent* PressedComponent, const UFigmaComponent* DisabledComponent, const UFigmaComponent* FocusedComponent) const;

private:

	UPROPERTY()
	TObjectPtr<UButton> Button = nullptr;

	FString PropertyName;
	FFigmaComponentPropertyDefinition PropertyDefinition;
};