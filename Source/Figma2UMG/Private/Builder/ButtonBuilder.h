// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Parser/Properties/FigmaComponentPropertyDefinition.h"

#include "ButtonBuilder.generated.h"

class UWidget;
class UButton;

USTRUCT()
struct FIGMA2UMG_API FButtonBuilder
{
public:
	GENERATED_BODY()

	TObjectPtr<UWidget> Patch(TObjectPtr<UWidget> WidgetToPatch, UObject* AssetOuter);
	void SetupWidget(TObjectPtr<UWidget> Widget);
	void Reset();
	void SetProperty(const FString& InPropertyName, const FFigmaComponentPropertyDefinition& InDefinition);

	TObjectPtr<UButton> GetWidget() const;
private:

	UPROPERTY()
	TObjectPtr<UButton> Button = nullptr;

	FString PropertyName;
	FFigmaComponentPropertyDefinition PropertyDefinition;
};