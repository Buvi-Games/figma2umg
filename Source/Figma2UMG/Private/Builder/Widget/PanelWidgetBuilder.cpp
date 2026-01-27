// MIT License
// Copyright (c) 2024 Buvi Games


#include "Builder/Widget/PanelWidgetBuilder.h"

#include "Figma2UMGModule.h"

void UPanelWidgetBuilder::SetWidget(const TObjectPtr<UWidget>& InWidget)
{
	UE_LOG_Figma2UMG(Display, TEXT("[UPanelWidgetBuilder::SetWidget] Node: %s, InWidget: %s (Type: %s)"),
		Node ? *Node->GetNodeName() : TEXT("no node"),
		InWidget ? *InWidget->GetName() : TEXT("NULL"),
		InWidget ? *InWidget->GetClass()->GetName() : TEXT("N/A"));
	Widget = Cast<UPanelWidget>(InWidget);
	if (!Widget && InWidget)
	{
		UE_LOG_Figma2UMG(Warning, TEXT("[UPanelWidgetBuilder::SetWidget] Cast to UPanelWidget failed! InWidget type: %s"), *InWidget->GetClass()->GetName());
	}
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
