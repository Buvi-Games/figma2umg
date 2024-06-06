// Copyright 2024 Buvi Games. All Rights Reserved.


#include "SizeBoxWidgetBuilder.h"


TObjectPtr<UWidget> USizeBoxWidgetBuilder::PatchPreInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
	return WidgetToPatch;
}
