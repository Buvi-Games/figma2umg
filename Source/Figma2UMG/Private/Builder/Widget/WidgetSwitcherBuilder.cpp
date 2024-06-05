// Copyright 2024 Buvi Games. All Rights Reserved.


#include "WidgetSwitcherBuilder.h"


void UWidgetSwitcherBuilder::AddChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder)
{
	ChildWidgetBuilders.Add(WidgetBuilder);
}