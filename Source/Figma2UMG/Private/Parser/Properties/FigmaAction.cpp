// MIT License
// Copyright (c) 2024 Buvi Games

#include "FigmaAction.h"

#include "Figma2UMGModule.h"
#include "JsonObjectConverter.h"

UFigmaAction* UFigmaAction::CreateAction(const TSharedPtr<FJsonObject>& ObjectJson)
{
	static FString TypeStr("type");
	if (!ObjectJson->HasTypedField<EJson::String>(TypeStr))
		return nullptr;

	const FString NodeTypeStr = ObjectJson->GetStringField(TypeStr);

	static const FString EnumPath = "/Script/Figma2UMG.EFigmaActionType";
	static UEnum* EnumDef = FindObject<UEnum>(nullptr, *EnumPath, true);
	if (!EnumDef)
		return nullptr;

	UFigmaAction* FigmaAction = nullptr;
	const EFigmaActionType ActionType = static_cast<EFigmaActionType>(EnumDef->GetValueByName(*NodeTypeStr));
	switch (ActionType)
	{
	case EFigmaActionType::BACK:
		FigmaAction = NewObject<UFigmaBackAction>();
		break;
	case EFigmaActionType::CLOSE:
		FigmaAction = NewObject<UFigmaCloseAction>();
		break;
	case EFigmaActionType::URL:
		FigmaAction = NewObject<UFigmaOpenURLAction>();
		break;
	case EFigmaActionType::UPDATE_MEDIA_RUNTIME:
		FigmaAction = NewObject<UFigmaUpdateMediaRuntimeAction>();
		break;
	case EFigmaActionType::NODE:
		FigmaAction = NewObject<UFigmaNodeAction>();
		break;
	case EFigmaActionType::SET_VARIABLE:
		FigmaAction = NewObject<UFigmaSetVariableAction>();
		break;
	case EFigmaActionType::SET_VARIABLE_MODE:
		FigmaAction = NewObject<UFigmaSetVariableModeAction>();
		break;
	case EFigmaActionType::CONDITIONAL:
		FigmaAction = NewObject<UFigmaConditionalAction>();
		break;
	}


	if (FigmaAction != nullptr && !FJsonObjectConverter::JsonObjectToUStruct(ObjectJson.ToSharedRef(), FigmaAction->GetClass(), FigmaAction))
	{
		UE_LOG_Figma2UMG(Error, TEXT("[UFigmaAction::CreateAction] Failed to parse Action of Type %s."), *NodeTypeStr);
	}

	return FigmaAction;
}
