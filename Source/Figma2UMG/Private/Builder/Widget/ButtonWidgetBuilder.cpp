// Copyright 2024 Buvi Games. All Rights Reserved.


#include "ButtonWidgetBuilder.h"

#include "Figma2UMGModule.h"
#include "Components/Button.h"
#include "Components/ContentWidget.h"


void UButtonWidgetBuilder::PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
	UE_LOG_Figma2UMG(Warning, TEXT("[UButtonWidgetBuilder::PatchAndInsertWidget] TODO."));
	Insert(WidgetTree, WidgetToPatch, WidgetToPatch);
}

TObjectPtr<UContentWidget> UButtonWidgetBuilder::GetContentWidget() const
{
	return Widget;
}
