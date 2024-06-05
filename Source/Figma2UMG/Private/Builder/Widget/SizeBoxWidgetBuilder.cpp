// Copyright 2024 Buvi Games. All Rights Reserved.


#include "SizeBoxWidgetBuilder.h"


void USizeBoxWidgetBuilder::SetChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder)
{
	SubWidgetBuilder = WidgetBuilder;
}