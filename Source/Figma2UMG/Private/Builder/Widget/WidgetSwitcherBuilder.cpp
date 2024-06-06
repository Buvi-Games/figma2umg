// Copyright 2024 Buvi Games. All Rights Reserved.


#include "WidgetSwitcherBuilder.h"

#include "Blueprint/WidgetTree.h"
#include "Components/WidgetSwitcher.h"
#include "Interfaces/WidgetOwner.h"
#include "Parser/Nodes/FigmaDocument.h"
#include "Parser/Nodes/FigmaNode.h"


void UWidgetSwitcherBuilder::AddChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder)
{
	ChildWidgetBuilders.Add(WidgetBuilder);
}

TObjectPtr<UWidget> UWidgetSwitcherBuilder::PatchPreInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
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

	TArray<UWidget*> AllChildren = Widget->GetAllChildren();
	TArray<UWidget*> NewChildren;
	for (const TScriptInterface<IWidgetBuilder>& ChildBuilder : ChildWidgetBuilders)
	{
		if(!ChildBuilder)
			continue;

		if (TObjectPtr<UWidget> ChildWidget = ChildBuilder->FindNodeWidgetInParent(Widget))
		{
			TObjectPtr<UWidget> SubWidget = ChildBuilder->PatchPreInsertWidget(WidgetTree, ChildWidget);
			if (SubWidget)
			{
				NewChildren.Add(SubWidget);
				Widget->AddChild(SubWidget);
			}
		}
	}

	AllChildren = Widget->GetAllChildren();
	for(int i = 0; i < AllChildren.Num() && AllChildren.Num() > NewChildren.Num(); i++)
	{
		if(NewChildren.Contains(AllChildren[i]))
			continue;

		AllChildren.RemoveAt(i);
	}

	return Widget;
}
