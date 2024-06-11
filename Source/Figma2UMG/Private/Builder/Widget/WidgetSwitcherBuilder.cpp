// Copyright 2024 Buvi Games. All Rights Reserved.


#include "WidgetSwitcherBuilder.h"

#include "FigmaImportSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Widget.h"
#include "Components/WidgetSwitcher.h"
#include "Parser/Nodes/FigmaNode.h"

void UWidgetSwitcherBuilder::PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
	Widget = Cast<UWidgetSwitcher>(WidgetToPatch);
	const FString NodeName = Node->GetNodeName();
	const FString WidgetName = Node->GetUniqueName();
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
		UFigmaImportSubsystem::TryRenameWidget(Node->GetUniqueName(), Widget);
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
