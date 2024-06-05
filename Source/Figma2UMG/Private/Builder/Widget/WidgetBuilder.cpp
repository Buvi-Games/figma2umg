// Copyright 2024 Buvi Games. All Rights Reserved.


#include "WidgetBuilder.h"

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
