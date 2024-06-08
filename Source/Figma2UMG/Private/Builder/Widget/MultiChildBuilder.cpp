// Copyright 2024 Buvi Games. All Rights Reserved.


#include "MultiChildBuilder.h"

#include "Figma2UMGModule.h"
#include "Blueprint/WidgetTree.h"
#include "Components/PanelWidget.h"
#include "Components/Spacer.h"
#include "Components/Widget.h"
#include "Parser/Nodes/FigmaNode.h"


void UMultiChildBuilder::AddChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder)
{
	ChildWidgetBuilders.Add(WidgetBuilder);
	if (WidgetBuilder)
	{
		WidgetBuilder->SetParent(this);
	}
}

bool UMultiChildBuilder::TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget)
{
	if (!PostPatchWidget)
	{
		UE_LOG_Figma2UMG(Warning, TEXT("[UMultiChildBuilder::TryInsertOrReplace] Trying to insert a <null> Widget at Node %s."), *Node->GetNodeName());
		return false;
	}
	const TObjectPtr<UPanelWidget> PanelWidget = GetPanelWidget();
	if (!PanelWidget)
	{
		UE_LOG_Figma2UMG(Warning, TEXT("[UMultiChildBuilder::TryInsertOrReplace] Node %s doesn't have ContentWidget."), *Node->GetNodeName());
		return false;
	}

	if (PanelWidget->HasChild(PostPatchWidget))
	{
		UE_LOG_Figma2UMG(Display, TEXT("[USingleChildBuilder::TryInsertOrReplace] Node %s ContentWidget %s alreay have Widget %s."), *Node->GetNodeName(), *PanelWidget->GetName(), *PostPatchWidget->GetName());
	}
	else if(PrePatchWidget != nullptr && PanelWidget->HasChild(PrePatchWidget))
	{
		UE_LOG_Figma2UMG(Display, TEXT("[UMultiChildBuilder::TryInsertOrReplace] Node %s ContentWidget %s replace OldWidget %s with NewWidget %s."), *Node->GetNodeName(), *PanelWidget->GetName(), *PrePatchWidget->GetName(), *PostPatchWidget->GetName());
		PanelWidget->ReplaceChild(PrePatchWidget, PostPatchWidget);
	}
	else
	{
		UE_LOG_Figma2UMG(Display, TEXT("[UMultiChildBuilder::TryInsertOrReplace] Node %s ContentWidget %s inserted Widget %s."), *Node->GetNodeName(), *PanelWidget->GetName(), *PostPatchWidget->GetName());
		PanelWidget->AddChild(PostPatchWidget);
	}
	return true;
}

TObjectPtr<UWidget> UMultiChildBuilder::GetWidget() const
{
	return GetPanelWidget();
}

void UMultiChildBuilder::PatchAndInsertChildren(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UPanelWidget>& ParentWidget)
{
	if (!ParentWidget)
	{
		UE_LOG_Figma2UMG(Warning, TEXT("[UMultiChildBuilder::PatchAndInsertChildren] ParentWidget is null at Node %s."), *Node->GetNodeName());
		return;
	}

	TArray<UWidget*> AllChildren = ParentWidget->GetAllChildren();
	TArray<UWidget*> NewChildren;
	for (const TScriptInterface<IWidgetBuilder>& ChildBuilder : ChildWidgetBuilders)
	{
		if (!ChildBuilder)
			continue;

		TObjectPtr<UWidget> ChildWidget = ChildBuilder->FindNodeWidgetInParent(ParentWidget);
		ChildBuilder->PatchAndInsertWidget(WidgetTree, ChildWidget);

		if (TObjectPtr<UWidget> PatchedWidget = ChildBuilder->GetWidget())
		{
			NewChildren.Add(PatchedWidget);
		}
	}

	AllChildren = ParentWidget->GetAllChildren();
	for (int i = 0; i < AllChildren.Num() && AllChildren.Num() > NewChildren.Num(); i++)
	{
		if(AllChildren[i] && AllChildren[i]->IsA<USpacer>())
			continue;

		if (NewChildren.Contains(AllChildren[i]))
			continue;
	
		AllChildren.RemoveAt(i);
	}
}

