// Copyright 2024 Buvi Games. All Rights Reserved.


#include "SizeBoxWidgetBuilder.h"


void USizeBoxWidgetBuilder::SetChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder)
{
	SubWidgetBuilder = WidgetBuilder;
}

TObjectPtr<UWidget> USizeBoxWidgetBuilder::PatchPreInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
	return WidgetToPatch;
}
