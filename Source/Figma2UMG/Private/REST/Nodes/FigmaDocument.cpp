// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/Nodes/FigmaDocument.h"

void UFigmaDocument::PostSerialize(const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(JsonObj);

	SerializeArray(Children, JsonObj, "Children");
}
