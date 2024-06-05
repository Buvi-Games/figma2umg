// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBuilder.generated.h"

class UFigmaNode;


UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class FIGMA2UMG_API UWidgetBuilder : public UInterface
{
	GENERATED_BODY()
};

class FIGMA2UMG_API IWidgetBuilder
{
	GENERATED_BODY()
public:
	UFUNCTION()
	virtual void SetNode(const UFigmaNode* InNode);

protected:
	const UFigmaNode* Node = nullptr;
};