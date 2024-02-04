// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/Nodes/FigmaCanvas.h"

#include "Components/CanvasPanel.h"


TObjectPtr<UWidget> UFigmaCanvas::PatchWidgetImp(TObjectPtr<UWidget> WidgetToPatch)
{
	UCanvasPanel* Canvas = nullptr;
	if(WidgetToPatch && WidgetToPatch->GetClass() == UCanvasPanel::StaticClass())
	{
		if (WidgetToPatch->GetName() != GetUniqueName())
		{
			WidgetToPatch->Rename(*GetUniqueName());
		}
		Canvas = Cast<UCanvasPanel>(WidgetToPatch);
	}
	else
	{
		Canvas = NewObject<UCanvasPanel>(GetAssetOuter(), *GetUniqueName());
	}


	return Canvas;
}

void UFigmaCanvas::PostInsert(UWidget* Widget) const
{
	Super::PostInsert(Widget);
	UCanvasPanel* Canvas = Widget ? Cast<UCanvasPanel>(Widget) : nullptr;
	if (Canvas)
	{
		AddOrPathChildren(Canvas, Children);
	}
}
