// Copyright 2024 Buvi Games. All Rights Reserved.


#include "PanelWidgetBuilder.h"

void UPanelWidgetBuilder::AddChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder)
{
	ChildWidgetBuilders.Add(WidgetBuilder);
}
