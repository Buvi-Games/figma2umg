// Copyright 2024 Buvi Games. All Rights Reserved.


#include "WidgetBuilder.h"

#include "Figma2UMGModule.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ContentWidget.h"
#include "Components/PanelWidget.h"
#include "Components/Widget.h"
#include "Parser/Nodes/FigmaNode.h"

void IWidgetBuilder::SetNode(const UFigmaNode* InNode)
{
	Node = InNode;
}

void IWidgetBuilder::SetParent(TScriptInterface<IWidgetBuilder> InParent)
{
	Parent = InParent;
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
	if (ChildWidgetBuilder)
	{
		ChildWidgetBuilder->SetParent(nullptr);
	}
	ChildWidgetBuilder = WidgetBuilder;
	if (ChildWidgetBuilder)
	{
		ChildWidgetBuilder->SetParent(this);
	}
}

void USingleChildBuilder::PatchPreInsertChild(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UContentWidget>& ParentWidget)
{
	if (!ParentWidget)
	{
		UE_LOG_Figma2UMG(Warning, TEXT("[USingleChildBuilder::PatchPreInsertChildren] ParentWidget is null at Node %s."), *Node->GetNodeName());
		return;
	}

	if(ChildWidgetBuilder)
	{
		TObjectPtr<UWidget> ChildWidget = ParentWidget->GetContent();
		TObjectPtr<UWidget> SubWidget = ChildWidgetBuilder->PatchPreInsertWidget(WidgetTree, ChildWidget);
		if (SubWidget)
		{
			ParentWidget->SetContent(SubWidget);
		}
	}
}

void UMultiChildBuilder::AddChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder)
{
	ChildWidgetBuilders.Add(WidgetBuilder);
	if (WidgetBuilder)
	{
		WidgetBuilder->SetParent(this);
	}
}

void UMultiChildBuilder::PatchPreInsertChildren(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UPanelWidget>& ParentWidget)
{
	if (!ParentWidget)
	{
		UE_LOG_Figma2UMG(Warning, TEXT("[UMultiChildBuilder::PatchPreInsertChildren] ParentWidget is null at Node %s."), *Node->GetNodeName());
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
