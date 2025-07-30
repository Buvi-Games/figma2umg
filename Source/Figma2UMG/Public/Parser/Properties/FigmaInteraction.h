// MIT License
// Copyright (c) 2024 Buvi Games

#pragma once

#include "CoreMinimal.h"
#include "FigmaEnums.h"

#include "FigmaInteraction.generated.h"

class UFigmaNodeAction;
class UFigmaTrigger;
class UFigmaAction;

USTRUCT()
struct FIGMA2UMG_API FFigmaInteraction
{
public:
	GENERATED_BODY()

	void PostSerialize(const TSharedPtr<FJsonObject> JsonObj);

	const UFigmaNodeAction* FindActionNode(const EFigmaActionNodeNavigation& Navigate) const;

	static FFigmaInteraction Invalid;
	bool IsValid() const;

	UPROPERTY()
	UFigmaTrigger* Trigger = nullptr;

	UPROPERTY()
	TArray<UFigmaAction*> Actions;
};