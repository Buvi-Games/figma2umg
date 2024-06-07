// Copyright 2024 Buvi Games. All Rights Reserved.


#include "WidgetSwitcherBuilder.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Widget.h"
#include "Components/WidgetSwitcher.h"
#include "Interfaces/WidgetOwner.h"
#include "Parser/Nodes/FigmaNode.h"

void UWidgetSwitcherBuilder::PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
	Widget = Cast<UWidgetSwitcher>(WidgetToPatch);
	if (Widget == nullptr)
	{
		Widget = IWidgetOwner::NewWidget<UWidgetSwitcher>(WidgetTree, *Node->GetUniqueName());
		if (WidgetToPatch)
		{
			Widget->AddChild(WidgetToPatch);
		}
	}
	else
	{
		UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
		UClass* ClassOverride = Importer ? Importer->GetOverrideClassForNode<UWidgetSwitcher>(Node->GetUniqueName()) : nullptr;
		if (ClassOverride && Widget->GetClass() != ClassOverride)
		{
			UWidgetSwitcher* NewSwitcher = IWidgetOwner::NewWidget<UWidgetSwitcher>(WidgetTree, *Node->GetUniqueName(), ClassOverride);
			while (Widget->GetChildrenCount() > 0)
			{
				NewSwitcher->AddChild(Widget->GetChildAt(0));
			}
			Widget = NewSwitcher;
		}
		IWidgetOwner::TryRenameWidget(Node->GetUniqueName(), Widget);
	}

	PatchAndInsertChildren(WidgetTree, Widget);

	Insert(WidgetTree, WidgetToPatch, Widget);
}

TObjectPtr<UPanelWidget> UWidgetSwitcherBuilder::GetPanelWidget()
{
	return Widget;
}
