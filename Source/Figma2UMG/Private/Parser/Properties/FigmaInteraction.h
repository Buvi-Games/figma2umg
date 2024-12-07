// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "FigmaInteraction.generated.h"

class UFigmaTrigger;
class UFigmaAction;

USTRUCT()
struct FIGMA2UMG_API FFigmaInteraction
{
public:
	GENERATED_BODY()

	void PostSerialize(const TSharedPtr<FJsonObject> JsonObj);

	UPROPERTY()
	UFigmaTrigger* Trigger;

	UPROPERTY()
	TArray<UFigmaAction*> Actions;
};