// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBuilder.generated.h"

class UFigmaNode;

UCLASS()
class UWidgetBuilder : public UObject
{
	GENERATED_BODY()

	void SetNode(const TObjectPtr<UFigmaNode>& InNode);

protected:
	UPROPERTY()
	TObjectPtr<UFigmaNode> Node = nullptr;
};
