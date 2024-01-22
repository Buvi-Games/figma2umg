// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/Nodes/FigmaGroup.h"

void UFigmaGroup::PostSerialize(const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(JsonObj);

	SerializeArray(Children, JsonObj, "Children");
}
