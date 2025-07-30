// MIT License
// Copyright (c) 2024 Buvi Games

#include "FigmaInteraction.h"

#include "FigmaAction.h"
#include "FigmaTrigger.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonTypes.h"

FFigmaInteraction FFigmaInteraction::Invalid = FFigmaInteraction();

void FFigmaInteraction::PostSerialize(const TSharedPtr<FJsonObject> JsonObj)
{
	static FString TriggerStr("trigger");
	if (JsonObj.IsValid() && JsonObj->HasTypedField<EJson::Object>(TriggerStr))
	{
		const TSharedPtr<FJsonObject>& ObjectJson = JsonObj->GetObjectField(TriggerStr);
		if (ObjectJson.IsValid())
		{
			Trigger =  UFigmaTrigger::CreateTrigger(ObjectJson);
		}
	}

	static FString ActionsStr("actions");
	if (JsonObj.IsValid() && JsonObj->HasTypedField<EJson::Array>(ActionsStr))
	{
		const TArray<TSharedPtr<FJsonValue>>& ArrayJson = JsonObj->GetArrayField(ActionsStr);
		for (int i = 0; i < ArrayJson.Num(); i++)
		{
			const TSharedPtr<FJsonValue>& ItemJson = ArrayJson[i];
			if (ItemJson.IsValid() && ItemJson->Type == EJson::Object)
			{
				const TSharedPtr<FJsonObject>& ObjectJson = ItemJson->AsObject();
				if (ObjectJson.IsValid())
				{
					UFigmaAction* Action = UFigmaAction::CreateAction(ObjectJson);
					if(Action)
					{
						Actions.Push(Action);
					}
				}
			}
		}
	}
}

const UFigmaNodeAction* FFigmaInteraction::FindActionNode(const EFigmaActionNodeNavigation& Navigate) const
{
	for (UFigmaAction* Action : Actions)
	{
		if(!Action || Action->Type != EFigmaActionType::NODE)
			continue;

		UFigmaNodeAction* ActionNode = Cast<UFigmaNodeAction>(Action);
		if(!ActionNode)
			continue;

		if (ActionNode->Navigation == Navigate)
			return ActionNode;
	}

	return nullptr;
}

bool FFigmaInteraction::IsValid() const
{
	if(this == &Invalid)
		return false;

	return (Trigger != nullptr || !Actions.IsEmpty());
}
