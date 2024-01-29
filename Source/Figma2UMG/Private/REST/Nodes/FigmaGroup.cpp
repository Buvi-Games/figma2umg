// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/Nodes/FigmaGroup.h"

void UFigmaGroup::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);

	SerializeArray(Children, JsonObj, "Children");
}
