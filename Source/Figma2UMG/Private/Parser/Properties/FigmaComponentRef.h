// Copyright 2024 Buvi Games. All Rights Reserved.

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
	FFigmaComponentSetRef* GetComponentSet() const { return FigmaComponentSetRef; }

	TObjectPtr<UWidgetBlueprintBuilder> GetAssetBuilder() const
	{
		if (FigmaComponentSetRef)
		{
			return FigmaComponentSetRef->GetAssetBuilder();
		}

		if (FigmaComponent)
		{
			return FigmaComponent->GetAssetBuilder();
		}

		return nullptr;
	}

protected:
	UPROPERTY()
	TObjectPtr<UFigmaComponent> FigmaComponent = nullptr;
	
	FFigmaComponentSetRef* FigmaComponentSetRef = nullptr;
};
