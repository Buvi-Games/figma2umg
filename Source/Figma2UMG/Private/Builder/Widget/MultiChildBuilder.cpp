// MIT License
// Copyright (c) 2024 Buvi Games


#include "Builder/Widget/MultiChildBuilder.h"

#include "Figma2UMGModule.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/PanelWidget.h"
#include "Components/Spacer.h"
#include "Components/Widget.h"
#include "Components/WrapBox.h"
#include "Parser/Nodes/FigmaGroup.h"
#include "Parser/Nodes/FigmaNode.h"


void UMultiChildBuilder::AddChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder)
{
	ChildWidgetBuilders.Add(WidgetBuilder);
	if (WidgetBuilder)
	{
		WidgetBuilder->SetParent(this);
	}
}

void UMultiChildBuilder::PostInsertWidgets(TObjectPtr<UWidgetBlueprint> WidgetBlueprint)
{
	for (const TScriptInterface<IWidgetBuilder>& ChildBuilder : ChildWidgetBuilders)
	{
		if (!ChildBuilder)
			continue;

		ChildBuilder->PostInsertWidgets(WidgetBlueprint);
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

void UMultiChildBuilder::PatchWidgetBinds(const TObjectPtr<UWidgetBlueprint>& WidgetBlueprint)
{
	IWidgetBuilder::PatchWidgetBinds(WidgetBlueprint);

	for (const TScriptInterface<IWidgetBuilder>& ChildBuilder : ChildWidgetBuilders)
	{
		if (!ChildBuilder)
			continue;

		ChildBuilder->PatchWidgetBinds(WidgetBlueprint);
	}
}

void UMultiChildBuilder::PatchWidgetProperties()
{
	for (const TScriptInterface<IWidgetBuilder>& ChildBuilder : ChildWidgetBuilders)
	{
		if (!ChildBuilder)
			continue;

		ChildBuilder->PatchWidgetProperties();
	}
}

TObjectPtr<UWidget> UMultiChildBuilder::GetWidget() const
{
	return GetPanelWidget();
}

TObjectPtr<UWidget> UMultiChildBuilder::FindWidgetRecursive(const FString& WidgetName) const
{
	TObjectPtr<UWidget> FoundWidget = IWidgetBuilder::FindWidgetRecursive(WidgetName);
	if (FoundWidget)
		return FoundWidget;

	for (const TScriptInterface<IWidgetBuilder>& ChildBuilder : ChildWidgetBuilders)
	{
		if (!ChildBuilder)
			continue;

		FoundWidget = ChildBuilder->FindWidgetRecursive(WidgetName);
		if (FoundWidget)
			return FoundWidget;
	}

	return nullptr;
}

void UMultiChildBuilder::ResetWidget()
{
	for (const TScriptInterface<IWidgetBuilder>& ChildBuilder : ChildWidgetBuilders)
	{
		if (!ChildBuilder)
			continue;

		ChildBuilder->ResetWidget();
	}
}

void UMultiChildBuilder::PatchAndInsertChildren(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UPanelWidget>& ParentWidget, TMap<FString, int32>& NameTracker)
{
	if (!ParentWidget)
	{
		UE_LOG_Figma2UMG(Warning, TEXT("[UMultiChildBuilder::PatchAndInsertChildren] ParentWidget is null at Node %s."), *Node->GetNodeName());
		return;
	}

	// Track which widgets have already been assigned to avoid duplicate matching
	TSet<UWidget*> AssignedWidgets;
	TArray<UWidget*> AllChildren = ParentWidget->GetAllChildren();
	TArray<UWidget*> NewChildren;

	for (const TScriptInterface<IWidgetBuilder>& ChildBuilder : ChildWidgetBuilders)
	{
		if (!ChildBuilder)
			continue;

		const UFigmaNode* ChildNode = ChildBuilder->GetNode();
		TObjectPtr<UWidget> ChildWidget = nullptr;

		if (ChildNode)
		{
			const FString IdForName = ChildNode->GetIdForName();
			const FString NodeName = ChildNode->GetNodeName();

			// First try to find by node ID
			for (TObjectPtr<UWidget> Widget : AllChildren)
			{
				if (!Widget || AssignedWidgets.Contains(Widget))
					continue;

				if (Widget->GetName().Contains(IdForName, ESearchCase::IgnoreCase))
				{
					ChildWidget = Widget;
					break;
				}
			}

			// Fallback: find first unassigned widget matching node name
			if (!ChildWidget)
			{
				for (TObjectPtr<UWidget> Widget : AllChildren)
				{
					if (!Widget || AssignedWidgets.Contains(Widget))
						continue;

					if (Widget->GetName().Contains(NodeName, ESearchCase::IgnoreCase))
					{
						ChildWidget = Widget;
						break;
					}
				}
			}

			if (ChildWidget)
			{
				AssignedWidgets.Add(ChildWidget);
			}
		}

		ChildBuilder->PatchAndInsertWidget(WidgetBlueprint, ChildWidget, NameTracker);

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

	FixSpacers(ParentWidget);
}

void UMultiChildBuilder::SetChildrenWidget(TObjectPtr<UPanelWidget> ParentWidget)
{
	if (!ParentWidget)
	{
		UE_LOG_Figma2UMG(Warning, TEXT("[UMultiChildBuilder::SetChildrenWidget] ParentWidget is null at Node %s."), *Node->GetNodeName());
		return;
	}

	UE_LOG_Figma2UMG(Display, TEXT("[UMultiChildBuilder::SetChildrenWidget] Node: %s, ParentWidget: %s, ChildBuilders: %d"),
		*Node->GetNodeName(), *ParentWidget->GetName(), ChildWidgetBuilders.Num());

	// Track which widgets have already been assigned to avoid duplicate matching
	TSet<UWidget*> AssignedWidgets;
	TArray<UWidget*> AllChildren = ParentWidget->GetAllChildren();

	for (const TScriptInterface<IWidgetBuilder>& ChildBuilder : ChildWidgetBuilders)
	{
		if (!ChildBuilder)
			continue;

		const UFigmaNode* ChildNode = ChildBuilder->GetNode();
		if (!ChildNode)
		{
			UE_LOG_Figma2UMG(Warning, TEXT("[UMultiChildBuilder::SetChildrenWidget] ChildBuilder has no node"));
			continue;
		}

		const FString IdForName = ChildNode->GetIdForName();
		const FString NodeName = ChildNode->GetNodeName();
		TObjectPtr<UWidget> ChildWidget = nullptr;

		// First try to find by node ID (for widgets that include the Figma ID in their name)
		for (TObjectPtr<UWidget> Widget : AllChildren)
		{
			if (!Widget || AssignedWidgets.Contains(Widget))
				continue;

			if (Widget->GetName().Contains(IdForName, ESearchCase::IgnoreCase))
			{
				ChildWidget = Widget;
				UE_LOG_Figma2UMG(Display, TEXT("[SetChildrenWidget] Found by ID: %s for node '%s'"), *Widget->GetName(), *NodeName);
				break;
			}
		}

		// Fallback: find first unassigned widget matching node name
		if (!ChildWidget)
		{
			for (TObjectPtr<UWidget> Widget : AllChildren)
			{
				if (!Widget || AssignedWidgets.Contains(Widget))
					continue;

				if (Widget->GetName().Contains(NodeName, ESearchCase::IgnoreCase))
				{
					ChildWidget = Widget;
					UE_LOG_Figma2UMG(Display, TEXT("[SetChildrenWidget] Found by name: %s for node '%s'"), *Widget->GetName(), *NodeName);
					break;
				}
			}
		}

		if (ChildWidget)
		{
			AssignedWidgets.Add(ChildWidget);
		}
		else
		{
			UE_LOG_Figma2UMG(Warning, TEXT("[SetChildrenWidget] No widget found for node '%s' (ID: %s)"), *NodeName, *IdForName);
		}

		UE_LOG_Figma2UMG(Display, TEXT("[UMultiChildBuilder::SetChildrenWidget] Found widget: %s for child builder type: %s"),
			ChildWidget ? *ChildWidget->GetName() : TEXT("NULL"),
			*ChildBuilder.GetObject()->GetClass()->GetName());
		ChildBuilder->SetWidget(ChildWidget);
	}
}


void UMultiChildBuilder::FixSpacers(const TObjectPtr<UPanelWidget>& PanelWidget) const
{
	if (!PanelWidget)
		return;

	float ItemSpacing = 0.0f;
	float CounterAxisSpacing = 0.0f;
	if (const UFigmaGroup* FigmaGroup = Cast<UFigmaGroup>(Node))
	{
		ItemSpacing = FigmaGroup->ItemSpacing;
		CounterAxisSpacing = FigmaGroup->CounterAxisSpacing;
	}

	if ((ItemSpacing == 0.0f && CounterAxisSpacing == 0.0f) || PanelWidget->IsA<UCanvasPanel>() || PanelWidget->IsA<UWrapBox>())
	{
		for (int i = 0; i < PanelWidget->GetChildrenCount(); i++)
		{
			UWidget* Widget = PanelWidget->GetChildAt(i);
			if (!Widget || Widget->IsA<USpacer>())
			{
				PanelWidget->RemoveChildAt(i);
				i--;
			}
		}
		if (UWrapBox* WrapBox = Cast<UWrapBox>(PanelWidget))
		{
			WrapBox->SetInnerSlotPadding(FVector2D(ItemSpacing, CounterAxisSpacing));
		}
	}
	else
	{
		for (int i = 0; i < PanelWidget->GetChildrenCount(); i++)
		{
			UWidget* Widget = PanelWidget->GetChildAt(i);
			const bool ShouldBeSpacer = (((i + 1) % 2) == 0);
			const bool IsSpacer = Widget && Widget->IsA<USpacer>();
			if (!Widget || (IsSpacer && !ShouldBeSpacer))
			{
				PanelWidget->RemoveChildAt(i);
				i--;
			}
			else if (ShouldBeSpacer && !IsSpacer)
			{
				USpacer* Spacer = NewObject<USpacer>(PanelWidget->GetOuter());
				Spacer->SetSize(FVector2D(ItemSpacing, ItemSpacing));
				PanelWidget->InsertChildAt(i, Spacer);
			}
			else if (ShouldBeSpacer && IsSpacer)
			{
				USpacer* Spacer = Cast<USpacer>(Widget);
				Spacer->SetSize(FVector2D(ItemSpacing, ItemSpacing));
			}
		}
	}

}
