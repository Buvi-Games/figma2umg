// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/FigmaFile.h"

void FFigmaFile::PostSerialize(const TSharedRef<FJsonObject> fileJsonObject)
{
	if (fileJsonObject->HasTypedField<EJson::Object>("components"))
	{
		const TSharedPtr<FJsonObject> Components = fileJsonObject->GetObjectField("components");		
		for (const TPair<FString, TSharedPtr<FJsonValue>>& Component : Components->Values)
		{
		}
	}

	if (fileJsonObject->HasTypedField<EJson::Object>("componentsSets"))
	{
		const TSharedPtr<FJsonObject> ComponentsSets = fileJsonObject->GetObjectField("componentsSets");
		for (const TPair<FString, TSharedPtr<FJsonValue>>& ComponentsSet : ComponentsSets->Values)
		{
		}
	}

	if (fileJsonObject->HasTypedField<EJson::Object>("styles"))
	{
		const TSharedPtr<FJsonObject> Styles = fileJsonObject->GetObjectField("styles");
		for (const TPair<FString, TSharedPtr<FJsonValue>>& Style : Styles->Values)
		{
		}
	}
}
