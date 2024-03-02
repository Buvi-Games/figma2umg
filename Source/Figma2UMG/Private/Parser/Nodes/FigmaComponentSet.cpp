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

void UFigmaComponentSet::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);

	static FString Hovered = FString("Hovered");
	static FString Pressed = FString("Pressed");

	for (const TPair<FString, FFigmaComponentPropertyDefinition>& Property : ComponentPropertyDefinitions)
	{
		if (Property.Value.Type == EFigmaComponentPropertyType::VARIANT)
		{
//			const bool hasHovered = Property.Value.VariantOptions.Find(Hovered) != INDEX_NONE;
//			const bool hasPressed = Property.Value.VariantOptions.Find(Pressed) != INDEX_NONE;
//			if (hasHovered && hasPressed)
//			{
//				IsButton = true;
//			}
//			else
			{
				FSwitcherBuilder& SwitcherBuilder = Builders.Add_GetRef(FSwitcherBuilder());
				SwitcherBuilder.SetProperty(Property.Key, Property.Value);
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
		CompileAndRefresh();
	}
}

TObjectPtr<UWidget> UFigmaComponentSet::PatchVariation(TObjectPtr<UWidget> WidgetToPatch)
{
	if (IsButton)
	{
		//TODO
		return WidgetToPatch;
	}

	const UWidgetBlueprint* WidgetBP = GetAsset<UWidgetBlueprint>();
	TObjectPtr<UWidgetSwitcher> TopWidgetSwitcher = nullptr;
	TObjectPtr<UWidgetSwitcher> ParentWidgetSwitcher = nullptr;
	for (FSwitcherBuilder& SwitcherBuilder : Builders)
	{
		if(TopWidgetSwitcher == nullptr)
		{
			SwitcherBuilder.Patch(WidgetToPatch, GetAssetOuter());
			TopWidgetSwitcher = SwitcherBuilder.GetWidgetSwitcher();
		}
		else
		{
			TArray<UWidget*> ChildrenWidgets = ParentWidgetSwitcher->GetAllChildren();
			if (UWidget** FoundChild = ChildrenWidgets.FindByPredicate([SwitcherBuilder](const UWidget* Widget) {return Widget && Widget->IsA<UWidget>() && Widget->GetName() == SwitcherBuilder.GetPropertyName(); }))
			{
				SwitcherBuilder.Patch((*FoundChild), GetAssetOuter());
			}
			else
			{
				TObjectPtr<UWidgetSwitcher> NewSwitcher = SwitcherBuilder.Patch(nullptr, GetAssetOuter());
				ParentWidgetSwitcher->AddChild(NewSwitcher);
				// TODO: Sort?
			}
		}
		ParentWidgetSwitcher = SwitcherBuilder.GetWidgetSwitcher();
	}

	for (TPair< FString, FFigmaComponentPropertyDefinition> PropertyDefinition : ComponentPropertyDefinitions)
	{
		for (int index = 0; index < PropertyDefinition.Value.VariantOptions.Num(); index++)
		{
			FString VariantOption = PropertyDefinition.Value.VariantOptions[index];
			FString ChildName = PropertyDefinition.Key + "=" + VariantOption;

			UFigmaNode** Found = Children.FindByPredicate([ChildName](UFigmaNode* Node) {return Node->GetNodeName() == ChildName; });
			if(Found)
			{
				UFigmaNode* Child = *Found;
				UFigmaComponent* Component = Cast<UFigmaComponent>(Child);
				if (!Component)
				{
					UE_LOG_Figma2UMG(Error, TEXT("UFigmaComponentSet %s has Child %s of type %s. Component type expected."), *GetNodeName(), *Child->GetNodeName(), *Child->GetClass()->GetDisplayNameText().ToString());
					continue;
				}

				if (TObjectPtr<UWidgetSwitcher> Switcher = FindSwitcher(PropertyDefinition.Key))
				{
					TObjectPtr<UWidget> OldWidget = Switcher->GetWidgetAtIndex(index);
					TObjectPtr<UWidget> NewWidget = Component->CreateInstance(WidgetBP->WidgetTree);
					if (OldWidget)
					{
						Switcher->ReplaceChildAt(index, NewWidget);
					}
					else
					{
						Switcher->AddChild(NewWidget);
					}
				}
				else
				{
					UE_LOG_Figma2UMG(Error, TEXT("UFigmaComponentSet %s can't fins UWidgetSwitcher %s."), *GetNodeName(), *PropertyDefinition.Key);
				}
			}
			else
			{
				UE_LOG_Figma2UMG(Error, TEXT("UFigmaComponentSet %s don't have expected Child %s."), *GetNodeName(), *ChildName);
			}
		}
	}

	return TopWidgetSwitcher;
	
}

TObjectPtr<UWidget> UFigmaComponentSet::PatchPreInsertWidget(TObjectPtr<UWidget> WidgetToPatch)
{
	UWidgetBlueprint* WidgetBP = GetAsset<UWidgetBlueprint>();
	WidgetBP->WidgetTree->RootWidget = PatchVariation(WidgetBP->WidgetTree->RootWidget);

	WidgetBP->WidgetTree->SetFlags(RF_Transactional);
	WidgetBP->WidgetTree->Modify();

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(WidgetBP);

	if (IsButton)
	{
		return nullptr;
	}
	else
	{
		IsDoingInPlace = true;
		TObjectPtr<UWidget> Widget = Super::PatchPreInsertWidget(WidgetToPatch);
		IsDoingInPlace = false;
		return Widget;
	}
}

UObject* UFigmaComponentSet::GetAssetOuter() const
{
	if (IsDoingInPlace && ParentNode)
	{
		return ParentNode->GetAssetOuter();
	}
	else
	{
		return Super::GetAssetOuter();
	}
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

void UFigmaComponentSet::CompileAndRefresh()
{
	UWidgetBlueprint* WidgetBP = GetAsset<UWidgetBlueprint>();
	if (!WidgetBP)
		return;

	Asset = nullptr;
	AssetOuter = nullptr;

	FCompilerResultsLog LogResults;
	LogResults.SetSourcePath(WidgetBP->GetPathName());
	LogResults.BeginEvent(TEXT("Compile"));
	LogResults.bLogDetailedResults = true;

	FKismetEditorUtilities::CompileBlueprint(WidgetBP, EBlueprintCompileOptions::None, &LogResults);

	GetOrCreateAsset<UWidgetBlueprint, UWidgetBlueprintFactory>();
}

bool UFigmaComponentSet::PatchPropertiesToWidget(UWidgetBlueprint* WidgetBP)
{
	bool AddedMemberVariable = false;
	for (const TPair<FString, FFigmaComponentPropertyDefinition> Property : ComponentPropertyDefinitions)
	{
		if(Property.Value.Type == EFigmaComponentPropertyType::VARIANT)
			continue;

		FEdGraphPinType MemberType;
		FillType(Property.Value, MemberType);
		FString PropertyName = Property.Key;//TODO: Remove '#id'
		if (FBlueprintEditorUtils::AddMemberVariable(WidgetBP, *PropertyName, MemberType, Property.Value.DefaultValue))
		{
			FBlueprintEditorUtils::SetBlueprintOnlyEditableFlag(WidgetBP, *PropertyName, false);
			AddedMemberVariable = true;
		}
	}

	for (FSwitcherBuilder& SwitcherBuilder : Builders)
	{
		SwitcherBuilder.AddVariation(WidgetBP);
		AddedMemberVariable = true;
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

TObjectPtr<UWidgetSwitcher> UFigmaComponentSet::FindSwitcher(const FString& SwitcherName) const
{
	TObjectPtr<UWidgetBlueprint> WidgetBp = GetAsset<UWidgetBlueprint>();
	TArray<UWidget*> Widgets;
	WidgetBp->WidgetTree->GetAllWidgets(Widgets);

	for (UWidget* Widget : Widgets)
	{
		UWidgetSwitcher* Switcher = Cast<UWidgetSwitcher>(Widget);
		if (!Switcher)
			continue;

		if (Switcher->GetName() == SwitcherName)
		{
			return Switcher;
		}
	}

	return nullptr;
}