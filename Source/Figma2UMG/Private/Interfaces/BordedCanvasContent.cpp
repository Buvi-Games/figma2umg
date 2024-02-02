// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/BordedCanvasContent.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"


TObjectPtr<UBorder> IBordedCanvasContent::AddOrPathContent(UBorder* BorderToPatch, UObject* Outer, const FString& UniqueName)
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

	return Border;
}

void IBordedCanvasContent::PostInsertContent(FLinearColor BrushColor /*= FLinearColor::White*/) const
{
	if (!Border)
		return;

	UCanvasPanelSlot* CanvasSlot = Border->Slot ? Cast<UCanvasPanelSlot>(Border->Slot) : nullptr;
	if (CanvasSlot)
	{
		//Todo: Make it relative to parent.
		//CanvasSlot->SetPosition(AbsoluteBoundingBox.GetPosition());
		CanvasSlot->SetPosition(GetPosition());
		CanvasSlot->SetSize(GetSize());
	}

	Border->SetBrushColor(BrushColor);
}
