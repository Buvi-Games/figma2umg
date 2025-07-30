// MIT License
// Copyright (c) 2024 Buvi Games

#include "FigmaTrigger.h"

#include "Figma2UMGModule.h"
#include "JsonObjectConverter.h"

UFigmaTrigger* UFigmaTrigger::CreateTrigger(const TSharedPtr<FJsonObject>& ObjectJson)
{
	static FString TypeStr("type");
	if (!ObjectJson->HasTypedField<EJson::String>(TypeStr))
		return nullptr;

	const FString NodeTypeStr = ObjectJson->GetStringField(TypeStr);

	static const FString EnumPath = "/Script/Figma2UMG.EFigmaTriggerType";
	static UEnum* EnumDef = FindObject<UEnum>(nullptr, *EnumPath, true);
	if (!EnumDef)
		return nullptr;

	UFigmaTrigger* FigmaTrigger = nullptr;
	const EFigmaTriggerType TriggerType = static_cast<EFigmaTriggerType>(EnumDef->GetValueByName(*NodeTypeStr));
	switch (TriggerType)
	{
	case EFigmaTriggerType::ON_CLICK:
	case EFigmaTriggerType::ON_HOVER:
	case EFigmaTriggerType::ON_PRESS: 
	case EFigmaTriggerType::ON_DRAG:
		FigmaTrigger = NewObject<UFigmaTrigger>();
		break;
	case EFigmaTriggerType::AFTER_TIMEOUT:
		FigmaTrigger = NewObject<UFigmaTriggerAfterTimeOut>();
		break;
	case EFigmaTriggerType::MOUSE_ENTER: 
	case EFigmaTriggerType::MOUSE_LEAVE: 
	case EFigmaTriggerType::MOUSE_UP: 
	case EFigmaTriggerType::MOUSE_DOWN:
		FigmaTrigger = NewObject<UFigmaTriggerMouse>();
		break;
	case EFigmaTriggerType::ON_MEDIA_HIT:
	case EFigmaTriggerType::ON_MEDIA_END:
		FigmaTrigger = NewObject<UFigmaTriggerMedia>();
		break;
	case EFigmaTriggerType::ON_KEY_DOWN: 
	case EFigmaTriggerType::ON_KEY_UP: 
		FigmaTrigger = NewObject<UFigmaTriggerKey>();
		break;
	}


	if (FigmaTrigger != nullptr && !FJsonObjectConverter::JsonObjectToUStruct(ObjectJson.ToSharedRef(), FigmaTrigger->GetClass(), FigmaTrigger))
	{
		UE_LOG_Figma2UMG(Error, TEXT("[UFigmaTrigger::CreateTrigger] Failed to parse Trigger of Type %s."), *NodeTypeStr);
	}

	return FigmaTrigger;
}

bool UFigmaTrigger::MatchEvent(FString EventName) const
{
	switch (Type)
	{
	case EFigmaTriggerType::ON_CLICK:
		return EventName.EndsWith("OnButtonClicked");
	case EFigmaTriggerType::ON_HOVER:
	case EFigmaTriggerType::ON_PRESS:
	case EFigmaTriggerType::ON_DRAG:
	case EFigmaTriggerType::AFTER_TIMEOUT:
	case EFigmaTriggerType::MOUSE_ENTER:
	case EFigmaTriggerType::MOUSE_LEAVE:
	case EFigmaTriggerType::MOUSE_UP:
	case EFigmaTriggerType::MOUSE_DOWN:
	case EFigmaTriggerType::ON_MEDIA_HIT:
	case EFigmaTriggerType::ON_MEDIA_END:
	case EFigmaTriggerType::ON_KEY_DOWN:
	case EFigmaTriggerType::ON_KEY_UP:
		return false;
	}
	return false;
}
