// Fill out your copyright notice in the Description page of Project Settings.


#include "Parser/Nodes/FigmaCanvas.h"

#include "Components/CanvasPanel.h"


TObjectPtr<UWidget> UFigmaCanvas::PatchPreInsertWidget(TObjectPtr<UWidget> WidgetToPatch)
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


	Super::PatchPreInsertWidget(Canvas);

	return Canvas;
}
