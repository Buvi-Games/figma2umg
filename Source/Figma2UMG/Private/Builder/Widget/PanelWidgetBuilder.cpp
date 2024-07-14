// Copyright 2024 Buvi Games. All Rights Reserved.


#include "PanelWidgetBuilder.h"

void UPanelWidgetBuilder::SetWidget(const TObjectPtr<UWidget>& InWidget)
{
	Widget = Cast<UPanelWidget>(InWidget);
	SetChildrenWidget(Widget);
}

void UPanelWidgetBuilder::ResetWidget()
{
	Super::ResetWidget();
	Widget = nullptr;
}

TObjectPtr<UPanelWidget> UPanelWidgetBuilder::GetPanelWidget() const
{
	return Widget;
}
