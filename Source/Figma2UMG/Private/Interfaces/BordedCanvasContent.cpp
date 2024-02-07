// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/BordedCanvasContent.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"


TObjectPtr<UBorder> IBordedCanvasContent::AddOrPatchContent(UBorder* BorderToPatch, UObject* Outer, const FString& UniqueName)
{
	Border = BorderToPatch;
	Canvas = nullptr;
	if (Border)
	{
		if (Border->GetName() != UniqueName)
		{
			Border->Rename(*UniqueName);
		}
		Canvas = Cast<UCanvasPanel>(Border->GetContent());
	}
	else
	{
		Border = NewObject<UBorder>(Outer, *UniqueName);
	}

	if (!Canvas)
	{
		Canvas = NewObject<UCanvasPanel>(Outer);
		Border->SetContent(Canvas);
	}


	Border->SetBrushColor(GetBrushColor());

	return Border;
}

void IBordedCanvasContent::ForEach(const FOnEachFunction& Function)
{
	if (!Function.IsBound())
		return;

	if (Border)
	{
		Function.ExecuteIfBound(*Border);
	}

	if (Canvas)
	{
		Function.ExecuteIfBound(*Canvas);
	}
}

void IBordedCanvasContent::PostInsert() const
{
	TObjectPtr<UWidget> TopWidget = GetTopWidget();
	if (!TopWidget)
		return;

	IWidgetOwner::PostInsert();

	if (UCanvasPanelSlot* CanvasSlot = TopWidget->Slot ? Cast<UCanvasPanelSlot>(TopWidget->Slot) : nullptr)
	{
		CanvasSlot->SetSize(GetSize());
	}
}

TObjectPtr<UWidget> IBordedCanvasContent::GetTopWidget() const
{
	return Border;
}

TObjectPtr<UPanelWidget> IBordedCanvasContent::GetContainerWidget() const
{
	return Canvas;
}
