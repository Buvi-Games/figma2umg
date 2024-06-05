// Copyright 2024 Buvi Games. All Rights Reserved.


#include "ButtonWidgetBuilder.h"


void UButtonWidgetBuilder::SetChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder)
{
	SubWidgetBuilder = WidgetBuilder;
}