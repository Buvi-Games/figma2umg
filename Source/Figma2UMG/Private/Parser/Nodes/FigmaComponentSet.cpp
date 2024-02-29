// Fill out your copyright notice in the Description page of Project Settings.


#include "Parser/Nodes/FigmaComponentSet.h"

#include "WidgetBlueprint.h"
#include "WidgetBlueprintFactory.h"
#include "Blueprint/WidgetTree.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Parser/FigmaFile.h"
#include "Parser/Properties/FigmaComponentRef.h"
#include "Templates/WidgetTemplateBlueprintClass.h"

void UFigmaComponentSet::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);

	static FString Hovered = FString("Hovered");
	static FString Pressed = FString("Pressed");

	for (const TPair<FString, FFigmaComponentPropertyDefinition>& pair : ComponentPropertyDefinitions)
	{
		if (pair.Value.Type == EFigmaComponentPropertyType::VARIANT)
		{
			const bool hasHovered = pair.Value.VariantOptions.Find(Hovered) != INDEX_NONE;
			const bool hasPressed = pair.Value.VariantOptions.Find(Pressed) != INDEX_NONE;
			if (hasHovered && hasPressed)
			{
//				IsButton = true;
			}
		}
	}

	TObjectPtr<UFigmaFile> FigmaFile = GetFigmaFile();
	FFigmaComponentSetRef* ComponentSetRef = FigmaFile->FindComponentSetRef(GetId());
	ComponentSetRef->SetComponentSet(this);
}

FString UFigmaComponentSet::GetPackagePath() const
{
	TObjectPtr<UFigmaNode> TopParentNode = ParentNode;
	while (TopParentNode && TopParentNode->GetParentNode())
	{
		TopParentNode = TopParentNode->GetParentNode();
	}

	return TopParentNode->GetCurrentPackagePath() + TEXT("/") + "Components";
}

FString UFigmaComponentSet::GetAssetName() const
{
	return GetUniqueName();
}

void UFigmaComponentSet::LoadOrCreateAssets(UFigmaFile* FigmaFile)
{
	UWidgetBlueprint* WidgetBP = GetOrCreateAsset<UWidgetBlueprint, UWidgetBlueprintFactory>();
	if (PatchPropertiesToWidget(WidgetBP))
	{
		FCompilerResultsLog LogResults;
		LogResults.SetSourcePath(WidgetBP->GetPathName());
		LogResults.BeginEvent(TEXT("Compile"));
		LogResults.bLogDetailedResults = true;

		FKismetEditorUtilities::CompileBlueprint(WidgetBP, EBlueprintCompileOptions::None, &LogResults);

		Asset = nullptr;
		WidgetBP = GetOrCreateAsset<UWidgetBlueprint, UWidgetBlueprintFactory>();
	}

	FFigmaComponentSetRef* ComponentSetRef = FigmaFile ? FigmaFile->FindComponentSetRef(GetId()) : nullptr;
	if (ComponentSetRef)
	{
		ComponentSetRef->SetAsset(GetAsset<UWidgetBlueprint>());
	}
}

TObjectPtr<UWidget> UFigmaComponentSet::Patch(TObjectPtr<UWidget> WidgetToPatch)
{
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher = Builder.Patch(WidgetToPatch, GetAssetOuter(), FString(""));

	UWidgetBlueprint* WidgetBP = GetAsset<UWidgetBlueprint>();
	for (const TPair<FString, FFigmaComponentPropertyDefinition> Property : ComponentPropertyDefinitions)
	{
		if(Property.Value.Type != EFigmaComponentPropertyType::VARIANT)
			continue;

	}

	return WidgetSwitcher;
}

TObjectPtr<UWidget> UFigmaComponentSet::PatchPreInsertWidget(TObjectPtr<UWidget> WidgetToPatch)
{
	UWidgetBlueprint* Widget = GetAsset<UWidgetBlueprint>();
	Widget->WidgetTree->RootWidget = Patch(Widget->WidgetTree->RootWidget);

	Widget->WidgetTree->SetFlags(RF_Transactional);
	Widget->WidgetTree->Modify();

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Widget);

	return IsButton ? nullptr : Super::PatchPreInsertWidget(WidgetToPatch);
}

void UFigmaComponentSet::PostInsert() const
{
	if (!IsButton)
	{
		Super::PostInsert();
	}
}

void UFigmaComponentSet::PatchBinds(TObjectPtr<UWidgetBlueprint> WidgetBp) const
{
	if (!IsButton)
	{
		Super::PatchBinds(WidgetBp);
	}
}

void UFigmaComponentSet::PrePatchWidget()
{
	if (!IsButton)
	{
		Super::PrePatchWidget();
	}
}

TArray<UFigmaNode*>& UFigmaComponentSet::GetChildren()
{
	if (IsButton)
	{
		return Empty;
	}

	return Super::GetChildren();
}

void UFigmaComponentSet::FillType(const FFigmaComponentPropertyDefinition& Def, FEdGraphPinType& MemberType) const
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
		break;
	}
}

bool UFigmaComponentSet::PatchPropertiesToWidget(UWidgetBlueprint* WidgetBP)
{
	bool AddedMemberVariable = false;
	for (const TPair<FString, FFigmaComponentPropertyDefinition> Property : ComponentPropertyDefinitions)
	{
		if(Property.Value.Type == EFigmaComponentPropertyType::VARIANT)
		{
			Builder.AddVariation(WidgetBP, Property.Key, Property.Value);
			AddedMemberVariable = true;
		}
		else
		{
			FEdGraphPinType MemberType;
			FillType(Property.Value, MemberType);
			FString PropertyName = Property.Key;//TODO: Remove '#id'
			if (FBlueprintEditorUtils::AddMemberVariable(WidgetBP, *PropertyName, MemberType, Property.Value.DefaultValue))
			{
				FBlueprintEditorUtils::SetBlueprintOnlyEditableFlag(WidgetBP, *PropertyName, false);
				AddedMemberVariable = true;
			}
		}
	}

	return AddedMemberVariable;
}

void UFigmaComponentSet::PatchBinds()
{
	TObjectPtr<UWidgetBlueprint> WidgetBp = GetAsset<UWidgetBlueprint>();
	if (!WidgetBp)
		return;

	Super::PatchBinds(WidgetBp);
}