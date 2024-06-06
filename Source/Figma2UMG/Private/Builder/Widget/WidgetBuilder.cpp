// Copyright 2024 Buvi Games. All Rights Reserved.


#include "WidgetBuilder.h"

#include "Figma2UMGModule.h"
#include "Blueprint/WidgetTree.h"
#include "Components/PanelWidget.h"
#include "Parser/Nodes/FigmaNode.h"

void IWidgetBuilder::SetNode(const UFigmaNode* InNode)
{
	Node = InNode;
}

TObjectPtr<UWidget> IWidgetBuilder::FindNodeWidgetInParent(const TObjectPtr<UPanelWidget>& ParentWidget) const
{
	if (!ParentWidget)
		return nullptr;

	TArray<UWidget*> AllChildren = ParentWidget->GetAllChildren();
	for (TObjectPtr<UWidget> Widget : AllChildren)
	{
		if (Widget == nullptr)
			continue;

		if (Widget->GetName().Contains(Node->GetIdForName(), ESearchCase::IgnoreCase))
		{
			return Widget;
		}
	}

	return nullptr;
}

void USingleChildBuilder::SetChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder)
{
	ChildWidgetBuilder = WidgetBuilder;
}

void USingleChildBuilder::PatchPreInsertChild(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UPanelWidget>& ParentWidget)
{
	//TODO
}

void UMultiChildBuilder::AddChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder)
{
	ChildWidgetBuilders.Add(WidgetBuilder);
}

void UMultiChildBuilder::PatchPreInsertChildren(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UPanelWidget>& ParentWidget)
{
	if (!ParentWidget)
	{
		UE_LOG_Figma2UMG(Display, TEXT("[UMultiChildBuilder::PatchPreInsertChildren] ParentWidget is null at Node %s."), *Node->GetNodeName());
		return;
	}

	TArray<UWidget*> AllChildren = ParentWidget->GetAllChildren();
	TArray<UWidget*> NewChildren;
	for (const TScriptInterface<IWidgetBuilder>& ChildBuilder : ChildWidgetBuilders)
	{
		if (!ChildBuilder)
			continue;

		TObjectPtr<UWidget> ChildWidget = ChildBuilder->FindNodeWidgetInParent(ParentWidget);
		TObjectPtr<UWidget> SubWidget = ChildBuilder->PatchPreInsertWidget(WidgetTree, ChildWidget);
		if (SubWidget)
		{
			NewChildren.Add(SubWidget);
			ParentWidget->AddChild(SubWidget);
		}
	}

	AllChildren = ParentWidget->GetAllChildren();
	for (int i = 0; i < AllChildren.Num() && AllChildren.Num() > NewChildren.Num(); i++)
	{
		if (NewChildren.Contains(AllChildren[i]))
			continue;

		AllChildren.RemoveAt(i);
	}
}
