// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "REST/Nodes/FigmaNode.h"

#include "FigmaDocument.generated.h"

class UWidgetBlueprint;

UCLASS()
class UFigmaDocument : public UFigmaNode
{
public:
	GENERATED_BODY()

	void PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj);

	void AddToAsset(UWidgetBlueprint* Widget) const;


	virtual FVector2D GetAbsolutePosition() const override { return FVector2D(); }
protected:
	UPROPERTY()
	TArray<UFigmaNode*> Children;
};
