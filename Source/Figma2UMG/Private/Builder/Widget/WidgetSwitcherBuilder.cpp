// Copyright 2024 Buvi Games. All Rights Reserved.


#include "WidgetSwitcherBuilder.h"

#include "FigmaImportSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Widget.h"
#include "Components/WidgetSwitcher.h"
#include "Parser/Nodes/FigmaInstance.h"
#include "Parser/Nodes/FigmaNode.h"

TObjectPtr<UWidget> UWidgetSwitcherBuilder::FindNodeWidgetInParent(const TObjectPtr<UPanelWidget>& ParentWidget) const
{
	if (!ParentWidget)
		return nullptr;

	FString IdForName = Node->GetUniqueName();
	const UFigmaInstance* FigmaInstance = Cast<UFigmaInstance>(Node);
	if (FigmaInstance && FigmaInstance->IsInstanceSwap())
	{

		const FString MainComponentStr("mainComponent");
		if (FigmaInstance->GetComponentPropertyReferences().Contains(MainComponentStr))
		{
			IdForName = FigmaInstance->GetComponentPropertyReferences()[MainComponentStr];
			int Index;
			if (IdForName.FindChar('#', Index))
			{
				IdForName.RightChopInline(Index+1);
			}
		}
	}

	TArray<UWidget*> AllChildren = ParentWidget->GetAllChildren();
	for (TObjectPtr<UWidget> ChildWidget : AllChildren)
	{
		if (ChildWidget == nullptr)
			continue;

		if (ChildWidget->GetName().Contains(IdForName, ESearchCase::IgnoreCase))
		{
			return ChildWidget;
		}
	}

	return nullptr;
}

void UWidgetSwitcherBuilder::PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
	Widget = Cast<UWidgetSwitcher>(WidgetToPatch);
	const FString NodeName = Node->GetNodeName();
	FString WidgetName = Node->GetUniqueName();
	const UFigmaInstance* FigmaInstance = Cast<UFigmaInstance>(Node);
	if(FigmaInstance && FigmaInstance->IsInstanceSwap())
	{

		const FString MainComponentStr("mainComponent");
		if (FigmaInstance->GetComponentPropertyReferences().Contains(MainComponentStr))
		{
			WidgetName = FigmaInstance->GetComponentPropertyReferences()[MainComponentStr];
		}
	}

	if (Widget == nullptr)
	{
		Widget = UFigmaImportSubsystem::NewWidget<UWidgetSwitcher>(WidgetTree, NodeName, WidgetName);
		if (WidgetToPatch)
		{
			Widget->AddChild(WidgetToPatch);
		}
	}
	else
	{
		UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
		UClass* ClassOverride = Importer ? Importer->GetOverrideClassForNode<UWidgetSwitcher>(NodeName) : nullptr;
		if (ClassOverride && Widget->GetClass() != ClassOverride)
		{
			UWidgetSwitcher* NewSwitcher = UFigmaImportSubsystem::NewWidget<UWidgetSwitcher>(WidgetTree, NodeName, WidgetName, ClassOverride);
			while (Widget->GetChildrenCount() > 0)
			{
				NewSwitcher->AddChild(Widget->GetChildAt(0));
			}
			Widget = NewSwitcher;
		}
		UFigmaImportSubsystem::TryRenameWidget(WidgetName, Widget);
	}

	PatchAndInsertChildren(WidgetTree, Widget);

	Insert(WidgetTree, WidgetToPatch, Widget);
}

void UWidgetSwitcherBuilder::SetWidget(const TObjectPtr<UWidget>& InWidget)
{
	Widget = Cast<UWidgetSwitcher>(InWidget);
	SetChildrenWidget(Widget);
}

void UWidgetSwitcherBuilder::ResetWidget()
{
	Super::ResetWidget();
	Widget = nullptr;
}

TObjectPtr<UPanelWidget> UWidgetSwitcherBuilder::GetPanelWidget() const
{
	return Widget;
}
