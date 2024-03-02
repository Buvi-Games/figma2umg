// Fill out your copyright notice in the Description page of Project Settings.


#include "Parser/Nodes/FigmaCanvas.h"

#include "Figma2UMGModule.h"
#include "Components/CanvasPanel.h"

void UFigmaCanvas::ForEach(const IWidgetOwner::FOnEachFunction& Function)
{
	Function.ExecuteIfBound(*Canvas);
}

TObjectPtr<UWidget> UFigmaCanvas::Patch(TObjectPtr<UWidget> WidgetToPatch)
{
	Canvas = nullptr;
	if (WidgetToPatch && WidgetToPatch->GetClass() == UCanvasPanel::StaticClass())
	{
		if (WidgetToPatch->GetName() != GetUniqueName())
		{
			WidgetToPatch->Rename(*GetUniqueName());
		}
		UE_LOG_Figma2UMG(Display, TEXT("%s Patching Canvas "), *GetUniqueName());
		Canvas = Cast<UCanvasPanel>(WidgetToPatch);
	}
	else
	{
		UE_LOG_Figma2UMG(Display, TEXT("%s New Canvas"), *GetUniqueName());
		Canvas = NewObject<UCanvasPanel>(GetAssetOuter(), *GetUniqueName());
	}

	return Canvas;
}

void UFigmaCanvas::Reset()
{
	Canvas = nullptr;
}

TObjectPtr<UWidget> UFigmaCanvas::GetTopWidget() const
{
	return Canvas;
}

FVector2D UFigmaCanvas::GetTopWidgetPosition() const
{
	return FVector2D::Zero();
}

TObjectPtr<UPanelWidget> UFigmaCanvas::GetContainerWidget() const
{
	return Canvas;
}

void UFigmaCanvas::PatchBinds(TObjectPtr<UWidgetBlueprint> WidgetBp) const
{
}
