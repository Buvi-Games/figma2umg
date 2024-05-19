// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/FigmaComponentSet.h"

#include "WidgetBlueprint.h"
#include "WidgetBlueprintFactory.h"
#include "Blueprint/WidgetTree.h"
#include "Builder/WidgetBlueprintBuilder.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Parser/FigmaFile.h"
#include "Parser/Properties/FigmaComponentRef.h"

void UFigmaComponentSet::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);
	GenerateFile = true;

	for (const TPair<FString, FFigmaComponentPropertyDefinition>& Property : ComponentPropertyDefinitions)
	{
		if (Property.Value.Type == EFigmaComponentPropertyType::VARIANT)
		{
			if (Property.Value.IsButton())
			{				
				FButtonBuilder& ButtonBuilder = ButtonBuilders.Add_GetRef(FButtonBuilder());
				ButtonBuilder.SetProperty(Property.Key, Property.Value);

				FString DefaultName = ButtonBuilder.GetDefaultName();
				UFigmaNode** FoundDefaultNode = Children.FindByPredicate([DefaultName](const UFigmaNode* Node) {return Node->GetNodeName().Compare(DefaultName, ESearchCase::IgnoreCase) == 0; });
				UFigmaComponent* DefaultComponent = FoundDefaultNode ? Cast<UFigmaComponent>(*FoundDefaultNode) : nullptr;
				if (DefaultComponent)
				{
					ButtonBuilder.SetDefaultComponent(DefaultComponent);
				}
			}
			else
			{
				FSwitcherBuilder& SwitcherBuilder = SwitchBuilders.Add_GetRef(FSwitcherBuilder());
				SwitcherBuilder.SetProperty(Property.Key, Property.Value);
			}
		}
		else
		{
			for (UFigmaNode* Child : Children)
			{
				UFigmaComponent* ChildComponent = Cast<UFigmaComponent>(Child);
				if (ChildComponent)
				{
					ChildComponent->TryAddComponentPropertyDefinition(Property.Key, Property.Value);
				}
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

void UFigmaComponentSet::LoadOrCreateAssets(UFigmaFile* FigmaFile)
{
	UWidgetBlueprint* WidgetBP = GetOrCreateAsset<UWidgetBlueprint, UWidgetBlueprintFactory>();
	if (PatchPropertiesToWidget(WidgetBP))
	{
		CompileBP(GetNodeName());
	}
}

TObjectPtr<UWidget> UFigmaComponentSet::GetTopWidget() const
{
	if (IsDoingInPlace)
	{
		return Super::GetTopWidget();
	}
	else
	{
		const UWidgetBlueprint* WidgetBP = GetAsset<UWidgetBlueprint>();
		return WidgetBP->WidgetTree->RootWidget;
	}
}

TObjectPtr<UPanelWidget> UFigmaComponentSet::GetContainerWidget() const
{
	if (IsDoingInPlace)
	{
		return Super::GetContainerWidget();
	}
	else
	{
		const UWidgetBlueprint* WidgetBP = GetAsset<UWidgetBlueprint>();
		for (const FSwitcherBuilder& SwitcherBuilder : SwitchBuilders)
		{
			//Todo: How to select the switch?
			if (const TObjectPtr<UPanelWidget> Container = SwitcherBuilder.GetWidgetSwitcher())
			{
				return Container;
			}
		}
		for (const FButtonBuilder& ButtonBuilder : ButtonBuilders)
		{
			if (const TObjectPtr<UPanelWidget> Container = ButtonBuilder.GetContainerWidget())
			{
				return Container;
			}
		}

		return nullptr;
	}
}

TObjectPtr<UWidget> UFigmaComponentSet::PatchVariation(TObjectPtr<UWidget> WidgetToPatch)
{
	const UWidgetBlueprint* WidgetBP = GetAsset<UWidgetBlueprint>();
	TObjectPtr<UWidgetSwitcher> TopWidgetSwitcher = nullptr;
	TObjectPtr<UWidgetSwitcher> ParentWidgetSwitcher = nullptr;

	//Todo: Properly handle Button + Switch
	for (FSwitcherBuilder& SwitcherBuilder : SwitchBuilders)
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

	for (FButtonBuilder& ButtonBuilder : ButtonBuilders)
	{
		if(ParentWidgetSwitcher)
		{
			//Todo: Find the proper child to replace
			WidgetToPatch = ButtonBuilder.Patch(ParentWidgetSwitcher->GetChildrenCount() == 0 ? nullptr : ParentWidgetSwitcher->GetChildAt(0), GetAssetOuter());
		}
		else
		{
			WidgetToPatch = ButtonBuilder.Patch(WidgetToPatch, GetAssetOuter());

			FString DefaultName = ButtonBuilder.GetDefaultName();
			FString HoveredName = ButtonBuilder.GetHoveredName();
			FString PressedName = ButtonBuilder.GetPressedName();
			FString DisabledName = ButtonBuilder.GetDisabledName();
			FString FocusedName = ButtonBuilder.GetFocusedName();

			UFigmaNode** FoundDefaultNode = Children.FindByPredicate([DefaultName](const UFigmaNode* Node) {return Node->GetNodeName().Compare(DefaultName, ESearchCase::IgnoreCase) == 0; });
			UFigmaNode** FoundHoveredNode = Children.FindByPredicate([HoveredName](const UFigmaNode* Node) {return Node->GetNodeName().Compare(HoveredName, ESearchCase::IgnoreCase) == 0; });
			UFigmaNode** FoundPressedNode = Children.FindByPredicate([PressedName](const UFigmaNode* Node) {return Node->GetNodeName().Compare(PressedName, ESearchCase::IgnoreCase) == 0; });
			UFigmaNode** FoundDisabledNode = Children.FindByPredicate([DisabledName](const UFigmaNode* Node) {return Node->GetNodeName().Compare(DisabledName, ESearchCase::IgnoreCase) == 0; });
			UFigmaNode** FoundFocusedNode = Children.FindByPredicate([FocusedName](const UFigmaNode* Node) {return Node->GetNodeName().Compare(FocusedName, ESearchCase::IgnoreCase) == 0; });

			UFigmaComponent* DefaultComponent = FoundDefaultNode ? Cast<UFigmaComponent>(*FoundDefaultNode) : nullptr;
			UFigmaComponent* HoveredComponent = FoundHoveredNode ? Cast<UFigmaComponent>(*FoundHoveredNode) : nullptr;
			UFigmaComponent* PressedComponent = FoundPressedNode ? Cast<UFigmaComponent>(*FoundPressedNode) : nullptr;
			UFigmaComponent* DisabledComponent = FoundDisabledNode ? Cast<UFigmaComponent>(*FoundDisabledNode) : nullptr;
			UFigmaComponent* FocusedComponent = FoundFocusedNode ? Cast<UFigmaComponent>(*FoundFocusedNode) : nullptr;

			ButtonBuilder.PatchStyle(DefaultComponent, HoveredComponent, PressedComponent, DisabledComponent, FocusedComponent);

		}
	}

	for (const TPair< FString, FFigmaComponentPropertyDefinition>& PropertyDefinition : ComponentPropertyDefinitions)
	{
		if (PropertyDefinition.Value.Type != EFigmaComponentPropertyType::VARIANT)
			continue;

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

				UWidgetBlueprint* ComponentAsset = Component ? Component->GetAsset<UWidgetBlueprint>() : nullptr;
				if (TObjectPtr<UWidgetSwitcher> Switcher = FindSwitcher(PropertyDefinition.Key))
				{
					TObjectPtr<UWidget> OldWidget = Switcher->GetWidgetAtIndex(index);
					if (OldWidget && OldWidget.GetClass()->ClassGeneratedBy == ComponentAsset)
					{
						OldWidget->bIsVariable = true;
						TryRenameWidget(Component->GetUniqueName(), OldWidget);
					}
					else
					{
						TObjectPtr<UWidget> NewWidget = Component->CreateInstance(WidgetBP->WidgetTree);
						NewWidget->bIsVariable = true;
						if (OldWidget)
						{
							Switcher->ReplaceChildAt(index, NewWidget);
						}
						else
						{
							Switcher->AddChild(NewWidget);
						}
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

	return TopWidgetSwitcher ? TopWidgetSwitcher : WidgetToPatch;
	
}

TObjectPtr<UWidget> UFigmaComponentSet::PatchPreInsertWidget(TObjectPtr<UWidget> WidgetToPatch)
{
	UWidgetBlueprint* WidgetBP = GetAsset<UWidgetBlueprint>();
	WidgetBP->WidgetTree->RootWidget = PatchVariation(WidgetBP->WidgetTree->RootWidget);

	WidgetBP->WidgetTree->SetFlags(RF_Transactional);
	WidgetBP->WidgetTree->Modify();

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(WidgetBP);

	if (!ButtonBuilders.IsEmpty())
	{
		return nullptr;
	}
	else if (GetAssetOuter())
	{
		IsDoingInPlace = true;
		//Jumping the UFigmaFrame::PatchPreInsertWidget since the Flow implementation
		TObjectPtr<UWidget> Widget = UFigmaGroup::PatchPreInsertWidget(WidgetToPatch);
		IsDoingInPlace = false;
		return Widget;
	}
	return nullptr;
}

void UFigmaComponentSet::SetWidget(TObjectPtr<UWidget> Widget)
{
	if (Widget)
	{
		UE_LOG_Figma2UMG(Display, TEXT("[SetWidget] UFigmaComponentSet %s received a UWidget %s of type %s."), *GetNodeName(), *Widget->GetName(), *Widget->GetClass()->GetDisplayNameText().ToString());
	}

	UWidgetBlueprint* WidgetBP = GetAsset<UWidgetBlueprint>();
	TArray<UWidget*> Widgets;
	WidgetBP->WidgetTree->GetAllWidgets(Widgets);
	if (!ButtonBuilders.IsEmpty())
	{
		//Todo: Properly handle Button + Switch
		for (FSwitcherBuilder& SwitcherBuilder : SwitchBuilders)
		{
			SwitcherBuilder.FindAndSetWidget(Widgets);
		}

		for (FButtonBuilder& ButtonBuilder : ButtonBuilders)
		{
			ButtonBuilder.SetupWidget(WidgetBP->WidgetTree->RootWidget);
		}
	}
	else
	{
		//Jumping the UFigmaFrame::SetWidget since the Flow implementation
		UFigmaGroup::SetWidget(Widget);

		for (FSwitcherBuilder& SwitcherBuilder : SwitchBuilders)
		{
			SwitcherBuilder.FindAndSetWidget(Widgets);
		}
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

void UFigmaComponentSet::Reset()
{
	Super::Reset();
	ResetAsset();
	for (FSwitcherBuilder& SwitcherBuilder : SwitchBuilders)
	{
		SwitcherBuilder.Reset();
	}
	for (FButtonBuilder& ButtonBuilder : ButtonBuilders)
	{
		ButtonBuilder.Reset();
	}
}

void UFigmaComponentSet::PostInsert() const
{
	if (ButtonBuilders.IsEmpty())
	{
		Super::PostInsert();
	}
	else
	{
		for (const FButtonBuilder& ButtonBuilder : ButtonBuilders)
		{
			ButtonBuilder.PostInsert();
		}
	}
}

void UFigmaComponentSet::PatchBinds(TObjectPtr<UWidgetBlueprint> WidgetBp) const
{
	if (ButtonBuilders.IsEmpty())
	{
		Super::PatchBinds(WidgetBp);
	}
	else
	{
		for (FButtonBuilder ButtonBuilder : ButtonBuilders)
		{
			if (UFigmaComponent* DefaultComponent = ButtonBuilder.GetDefaultComponent())
			{
				DefaultComponent->PatchBinds(WidgetBp);
			}
		}
	}

	for (const TPair< FString, FFigmaComponentPropertyDefinition>& PropertyDefinition : ComponentPropertyDefinitions)
	{
		if (PropertyDefinition.Value.Type == EFigmaComponentPropertyType::VARIANT)
			continue;

		PatchInitFunction(PropertyDefinition);
	}
}

void UFigmaComponentSet::PrePatchWidget()
{
	if (ButtonBuilders.IsEmpty())
	{
		Super::PrePatchWidget();
	}
}

TArray<UFigmaNode*>& UFigmaComponentSet::GetChildren()
{
	if (!ButtonBuilders.IsEmpty())
	{
		ButtonSubNodes.Reset();

		for (FButtonBuilder& ButtonBuilder : ButtonBuilders)
		{
			if (UFigmaComponent* DefaultComponent = ButtonBuilder.GetDefaultComponent())
			{
				ButtonSubNodes.Append(DefaultComponent->GetChildren());
			}
		}

		return ButtonSubNodes;
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
			continue;

		FEdGraphPinType MemberType;
		FillType(Property.Value, MemberType);
		FString PropertyName = Property.Key;//TODO: Remove '#id'
		if (FBlueprintEditorUtils::AddMemberVariable(WidgetBP, *PropertyName, MemberType, Property.Value.DefaultValue))
		{
			FBlueprintEditorUtils::SetBlueprintOnlyEditableFlag(WidgetBP, *PropertyName, false);
			AddedMemberVariable = true;
		}

		TSet<FName> CurrentVars;
		FBlueprintEditorUtils::GetClassVariableList(WidgetBP, CurrentVars);
		if (AddedMemberVariable || CurrentVars.Contains(*PropertyName))
		{
			FString FunctionName = "Init" + PropertyName;
			TObjectPtr<UEdGraph>* Graph = WidgetBP->FunctionGraphs.FindByPredicate([FunctionName](const TObjectPtr<UEdGraph> Graph)
				{
					return Graph.GetName() == FunctionName;
				});

			UEdGraph* FunctionGraph = Graph ? *Graph : nullptr;
			if (!FunctionGraph)
			{
				FunctionGraph = FBlueprintEditorUtils::CreateNewGraph(WidgetBP, FBlueprintEditorUtils::FindUniqueKismetName(WidgetBP, FunctionName), UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());

				FBlueprintEditorUtils::AddFunctionGraph<UClass>(WidgetBP, FunctionGraph, true, nullptr);
				AddedMemberVariable = true;
			}
		}
		else
		{
			UE_LOG_Figma2UMG(Error, TEXT("[UFigmaComponentSet::PatchPropertiesToWidget] Fail to add member variable %s of type %s."), *PropertyName, *MemberType.PinCategory.ToString());
		}
		
	}

	for (FSwitcherBuilder& SwitcherBuilder : SwitchBuilders)
	{
		SwitcherBuilder.AddVariation(WidgetBP);
		AddedMemberVariable = true;
	}

	return AddedMemberVariable;
}

void UFigmaComponentSet::PatchInitFunction(const TPair< FString, FFigmaComponentPropertyDefinition>& PropertyDefinition) const
{
	if (PropertyDefinition.Value.Type == EFigmaComponentPropertyType::VARIANT)
		return;

	FString PropertyName = PropertyDefinition.Key;
	FString FunctionName = "Init" + PropertyName;
	WidgetBlueprintBuilder::CallFunctionFromEventNode(GetAsset<UWidgetBlueprint>(), "PreConstruct", FunctionName);

	const TObjectPtr<UWidgetBlueprint> WidgetBP = GetAsset<UWidgetBlueprint>();
	if (!WidgetBP)
	{
		UE_LOG_Figma2UMG(Error, TEXT("[UFigmaComponentSet::PatchInitFunction] GetAsset<UWidgetBlueprint> in Node %s is nullptr."), *GetNodeName());
		return;
	}

	TObjectPtr<UWidgetBlueprint> WidgetBp = GetAsset<UWidgetBlueprint>();
	TArray<UWidget*> Widgets;
	WidgetBp->WidgetTree->GetAllWidgets(Widgets);

	bool Patched = false;
	for (UWidget* Widget : Widgets)
	{
		UWidgetSwitcher* Switcher = Cast<UWidgetSwitcher>(Widget);
		if (!Switcher)
			continue;

		WidgetBlueprintBuilder::PatchInitFunction(WidgetBP, Switcher, PropertyName);
		Patched = true;
	}

	if (!Patched)
	{
		UE_LOG_Figma2UMG(Error, TEXT("[UFigmaComponentSet::PatchInitFunction] Can't find UWidgetSwitcher for property %s in Node %s."), *PropertyDefinition.Key, *GetNodeName());
	}
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

		if (Switcher->GetName().Contains(SwitcherName))
		{
			return Switcher;
		}
	}

	return nullptr;
}