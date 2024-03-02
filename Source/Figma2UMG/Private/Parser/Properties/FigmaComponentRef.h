// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FigmaComponentSetRef.h"
#include "FigmaDocumentationLink.h"
#include "FigmaReference.h"
#include "Parser/Nodes/FigmaComponent.h"

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

	void SetComponentSet(FFigmaComponentSetRef* Value) { FigmaComponentSetRef = Value; }

	TObjectPtr<UWidgetBlueprint>  GetAsset() const
	{
		if (FigmaComponentSetRef)
		{
			return FigmaComponentSetRef->GetAsset();
		}

		if (FigmaComponent)
		{
			return FigmaComponent->GetAsset<UWidgetBlueprint>();
		}

		return nullptr;
	}

protected:
	UPROPERTY()
	TObjectPtr<UFigmaComponent> FigmaComponent = nullptr;

	FFigmaComponentSetRef* FigmaComponentSetRef = nullptr;
};
