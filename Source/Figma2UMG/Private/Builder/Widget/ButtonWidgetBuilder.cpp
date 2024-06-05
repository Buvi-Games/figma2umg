// Copyright 2024 Buvi Games. All Rights Reserved.


#include "ButtonWidgetBuilder.h"


void UButtonWidgetBuilder::SetChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder)
{
	SubWidgetBuilder = WidgetBuilder;
}

TObjectPtr<UWidget> UButtonWidgetBuilder::PatchPreInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
	return WidgetToPatch;
}
