// Copyright 2024 Buvi Games. All Rights Reserved.


#include "PanelWidgetBuilder.h"

void UPanelWidgetBuilder::AddChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder)
{
	ChildWidgetBuilders.Add(WidgetBuilder);
}

TObjectPtr<UWidget> UPanelWidgetBuilder::PatchPreInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
	return WidgetToPatch;
}
