// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FigmaDocumentationLink.h"
#include "FigmaReference.h"
#include "Parser/Nodes/FigmaComponentSet.h"

#include "FigmaComponentSetRef.generated.h"

USTRUCT()
struct FIGMA2UMG_API FFigmaComponentSetRef : public FFigmaReference
{
public:
	GENERATED_BODY()

	void SetComponentSet(TObjectPtr<UFigmaComponentSet> Value) { FigmaComponentSet = Value; }
	TObjectPtr<UFigmaComponentSet>  GetComponentSet() const { return FigmaComponentSet; }

	void SetAsset(TObjectPtr<UWidgetBlueprint> Value) { ComponentAsset = Value; }
	TObjectPtr<UWidgetBlueprint>  GetAsset() const { return ComponentAsset; }

protected:
	UPROPERTY()
	TArray<FFigmaDocumentationLink> DocumentationLinks;

	UPROPERTY()
	TObjectPtr<UFigmaComponentSet> FigmaComponentSet = nullptr;

	UPROPERTY()
	TObjectPtr<UWidgetBlueprint> ComponentAsset = nullptr;
};
