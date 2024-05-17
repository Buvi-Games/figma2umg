// Copyright 2024 Buvi Games. All Rights Reserved.

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

	TObjectPtr<UWidgetBlueprint>  GetAsset() const { return FigmaComponentSet ? FigmaComponentSet->GetAsset<UWidgetBlueprint>() : nullptr; }

protected:
	UPROPERTY()
	TArray<FFigmaDocumentationLink> DocumentationLinks;

	UPROPERTY()
	TObjectPtr<UFigmaComponentSet> FigmaComponentSet = nullptr;
};
