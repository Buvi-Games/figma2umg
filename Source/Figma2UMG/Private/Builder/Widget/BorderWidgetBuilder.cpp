// Copyright 2024 Buvi Games. All Rights Reserved.


#include "BorderWidgetBuilder.h"


void UBorderWidgetBuilder::SetChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder)
{
	SubWidgetBuilder = WidgetBuilder;
}
