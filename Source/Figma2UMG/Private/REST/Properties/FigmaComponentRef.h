// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FigmaDocumentationLink.h"
#include "FigmaReference.h"
#include "REST/Nodes/FigmaComponent.h"

#include "FigmaComponentRef.generated.h"

USTRUCT()
struct FIGMA2UMG_API FFigmaComponentRef : public FFigmaReference
{
public:
	GENERATED_BODY()

	UPROPERTY()
	FString ComponentSetId;

	UPROPERTY()
	TArray<FFigmaDocumentationLink> DocumentationLinks;

	void SetComponent(TObjectPtr<UFigmaComponent> Value) { FigmaComponent = Value; }
	TObjectPtr<UFigmaComponent>  GetComponent() const { return FigmaComponent; }

	void SetAsset(TObjectPtr<UWidgetBlueprint> Value) { ComponentAsset = Value; }
	TObjectPtr<UWidgetBlueprint>  GetAsset() const { return ComponentAsset; }

protected:
	UPROPERTY()
	TObjectPtr<UFigmaComponent> FigmaComponent = nullptr;

	UPROPERTY()
	TObjectPtr<UWidgetBlueprint> ComponentAsset = nullptr;
};
