// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "REST/Nodes/FigmaNode.h"

#include "FigmaDocument.generated.h"

UCLASS()
class UFigmaDocument : public UFigmaNode
{
public:
	GENERATED_BODY()

	void PostSerialize(const TSharedRef<FJsonObject> JsonObj);

protected:
	UPROPERTY()
	TArray<UFigmaNode*> Children;
};
