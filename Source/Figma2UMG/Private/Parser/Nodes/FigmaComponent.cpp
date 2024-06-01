// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/FigmaComponent.h"

#include "FigmaInstance.h"
#include "WidgetBlueprint.h"
#include "WidgetBlueprintFactory.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Parser/FigmaFile.h"
#include "Parser/Properties/FigmaComponentRef.h"
#include "Templates/WidgetTemplateBlueprintClass.h"

void UFigmaComponent::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);
	GenerateFile = true;

	TObjectPtr<UFigmaFile> FigmaFile = GetFigmaFile();
	FFigmaComponentRef* ComponentRef = FigmaFile->FindComponentRef(GetId());
	ComponentRef->SetComponent(this);
}

void UFigmaComponent::Reset()
{
	Super::Reset();
	ResetAsset();
	InstanceAsset = nullptr;
}

FString UFigmaComponent::GetPackageName() const
{
	return GetPackagePath();
}

FString UFigmaComponent::GetPackagePath() const
{
	TObjectPtr<UFigmaNode> TopParentNode = ParentNode;
	while (TopParentNode && TopParentNode->GetParentNode())
	{
		TopParentNode = TopParentNode->GetParentNode();
	}

	return TopParentNode->GetCurrentPackagePath() + TEXT("/") + "Components";
}

void UFigmaComponent::LoadOrCreateAssets()
{
	UWidgetBlueprint* WidgetBP = GetOrCreateWidgetBlueprint();
	if (PatchPropertiesToWidget(WidgetBP))
	{
		CompileBP(GetNodeName());
	}

	RefAsset = WidgetBP;
}

void UFigmaComponent::LoadAssets()
{
	RefAsset = LoadAsset<UWidgetBlueprint>();
}

void UFigmaComponent::FillType(const FFigmaComponentPropertyDefinition& Def, FEdGraphPinType& MemberType) const
{
	MemberType.ContainerType = EPinContainerType::None;
	switch (Def.Type)
	{
	case EFigmaComponentPropertyType::BOOLEAN:
		MemberType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
		break;
	case EFigmaComponentPropertyType::TEXT:
		MemberType.PinCategory = UEdGraphSchema_K2::PC_String;

		break;
	case EFigmaComponentPropertyType::INSTANCE_SWAP:
		MemberType.PinCategory = UEdGraphSchema_K2::PC_Object;
		// MemberType.PinSubCategory = ?
		// MemberType.PinSubCategoryObject = ?
		break;
	case EFigmaComponentPropertyType::VARIANT:
		//TODO:
		break;
	}
}

bool UFigmaComponent::PatchPropertiesToWidget(UWidgetBlueprint* Widget) const
{
	bool AddedMemberVariable = false;
	for(const TPair<FString, FFigmaComponentPropertyDefinition> Property : ComponentPropertyDefinitions)
	{
		FEdGraphPinType MemberType;
		FillType(Property.Value, MemberType);
		FString PropertyName = Property.Key;//TODO: Remove '#id'
		if (FBlueprintEditorUtils::AddMemberVariable(Widget, *PropertyName, MemberType, Property.Value.DefaultValue))
		{
			FBlueprintEditorUtils::SetBlueprintOnlyEditableFlag(Widget, *PropertyName, false);
			AddedMemberVariable = true;
		}
	}

	return AddedMemberVariable;
}

void UFigmaComponent::TryAddComponentPropertyDefinition(FString PropertyId, FFigmaComponentPropertyDefinition Definition)
{
	if (ComponentPropertyDefinitions.Contains(PropertyId))
		return;

	ComponentPropertyDefinitions.Add(PropertyId, Definition);
}

void UFigmaComponent::PatchBinds(TObjectPtr<UWidgetBlueprint> WidgetBp) const
{
	Super::PatchBinds(WidgetBp);
}
