// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/Nodes/FigmaSection.h"

void UFigmaSection::PostSerialize(const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(JsonObj);

	if (JsonObj->HasTypedField<EJson::Object>("devStatus"))
	{
		const TSharedPtr<FJsonObject> DevStatusJson = JsonObj->GetObjectField("devStatus");
		if (DevStatusJson->HasTypedField<EJson::String>("type"))
		{
			DevStatus = JsonObj->GetStringField("type");
		}
	}

	SerializeArray(Children, JsonObj, "Children");
}
