// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/WidgetOwner.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Widget.h"

void IWidgetOwner::PostInsert() const
{
	const UWidget* Widget = GetTopWidget();
	if (Widget && Widget->Slot)
	{
		UCanvasPanelSlot* CanvasSlot = Widget->Slot ? Cast<UCanvasPanelSlot>(Widget->Slot) : nullptr;
		if (CanvasSlot)
		{
			CanvasSlot->SetPosition(GetTopWidgetPosition());
		}
	}
}
