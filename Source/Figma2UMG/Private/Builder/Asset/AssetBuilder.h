// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetBuilder.generated.h"

class UFigmaNode;

UCLASS()
class UAssetBuilder : public UObject
{
	GENERATED_BODY()

	virtual void SetNode(const TObjectPtr<UFigmaNode>& InNode);

protected:
	UPROPERTY()
	TObjectPtr<UFigmaNode> Node = nullptr;
};
