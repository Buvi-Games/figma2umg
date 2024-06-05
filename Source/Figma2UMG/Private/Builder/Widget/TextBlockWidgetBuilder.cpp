// Copyright 2024 Buvi Games. All Rights Reserved.


#include "TextBlockWidgetBuilder.h"

TObjectPtr<UWidget> UTextBlockWidgetBuilder::PatchPreInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
	return WidgetToPatch;
}
