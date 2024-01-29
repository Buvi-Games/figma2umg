// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/Nodes/FigmaComponentSet.h"


void UFigmaComponentSet::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);
}