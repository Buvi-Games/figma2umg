// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/WidgetOwner.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Widget.h"
#include "Components/WrapBoxSlot.h"

void IWidgetOwner::PostInsert() const
{
	const TObjectPtr<UWidget> Widget = GetTopWidget();
	SetPosition(Widget, GetTopWidgetPosition());
}

void IWidgetOwner::SetPosition(TObjectPtr<UWidget> Widget, const FVector2D& Position) const
{
	if (Widget && Widget->Slot)
	{
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
		{
			CanvasSlot->SetPosition(Position);
//			CanvasSlot->SetAutoSize(true);
		}
		//else if (UHorizontalBoxSlot* HorizontalBoxSlot = Cast<UHorizontalBoxSlot>(Widget->Slot))
		//{
		//	HorizontalBoxSlot->SetPosition(Position);
		//}
		//else if (UVerticalBoxSlot* VerticalBoxSlot = Cast<UVerticalBoxSlot>(Widget->Slot))
		//{
		//	VerticalBoxSlot->SetPosition(Position);
		//}
		//else if (UWrapBoxSlot* WrapBoxSlot = Cast<UWrapBoxSlot>(Widget->Slot))
		//{
		//	WrapBoxSlot->SetPosition(Position);
		//}
	}
}

void IWidgetOwner::SetSize(TObjectPtr<UWidget> Widget, const FVector2D& Size, const bool SizeToContent /*= false*/) const
{
	if (Widget && Widget->Slot)
	{
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
		{
			CanvasSlot->SetSize(Size);
			if (SizeToContent)
			{
				CanvasSlot->SetAutoSize(true);
			}
		}
		//else if (UHorizontalBoxSlot* HorizontalBoxSlot = Cast<UHorizontalBoxSlot>(Widget->Slot))
		//{
		//	HorizontalBoxSlot->SetSize(Size);
		//}
		//else if (UVerticalBoxSlot* VerticalBoxSlot = Cast<UVerticalBoxSlot>(Widget->Slot))
		//{
		//	VerticalBoxSlot->SetSize(Size);
		//}
		//else if (UWrapBoxSlot* WrapBoxSlot = Cast<UWrapBoxSlot>(Widget->Slot))
		//{
		//	WrapBoxSlot->SetSize(Size);
		//}
	}
}
