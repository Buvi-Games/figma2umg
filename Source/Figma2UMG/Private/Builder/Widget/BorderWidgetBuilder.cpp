// Copyright 2024 Buvi Games. All Rights Reserved.


#include "BorderWidgetBuilder.h"


TObjectPtr<UWidget> UBorderWidgetBuilder::PatchPreInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
	return WidgetToPatch;
}
