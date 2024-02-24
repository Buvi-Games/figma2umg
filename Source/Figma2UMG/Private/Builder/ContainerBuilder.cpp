// Fill out your copyright notice in the Description page of Project Settings.


#include "Builder/ContainerBuilder.h"

#include "Components/Border.h"
#include "Components/CanvasPanel.h"

void FContainerBuilder::ForEach(const IWidgetOwner::FOnEachFunction& Function)
{
	if (!Function.IsBound())
		return;

	Super::ForEach(Function);

	if (Canvas)
	{
		Function.ExecuteIfBound(*Canvas);
	}
}

void FContainerBuilder::SetLayout(EFigmaLayoutMode InLayoutMode, EFigmaLayoutWrap InLayoutWrap)
{
	LayoutMode = InLayoutMode;
	LayoutWrap = InLayoutWrap;
}

TObjectPtr<UWidget> FContainerBuilder::Patch(TObjectPtr<UWidget> WidgetToPatch, UObject* AssetOuter, const FString& WidgetName)
{
	Super::Patch(WidgetToPatch, AssetOuter, WidgetName);
	Canvas = nullptr;

	TObjectPtr<UBorder> BorderWrapper = GetBorder();
	if (BorderWrapper)
	{
		Canvas = Cast<UCanvasPanel>(BorderWrapper->GetContent());
		if (!Canvas)
		{
			Canvas = NewObject<UCanvasPanel>(AssetOuter);
			BorderWrapper->SetContent(Canvas);
		}
	}
	else
	{
		Canvas = Cast<UCanvasPanel>(WidgetToPatch);
	}

	if(BorderWrapper)
	{
		return BorderWrapper;
	}
	else
	{
		return Canvas;
	}
}

void FContainerBuilder::Reset()
{
	Super::Reset();
	Canvas = nullptr;
}

TObjectPtr<UWidget> FContainerBuilder::GetTopWidget() const
{
	TObjectPtr<UBorder> BorderWrapper = GetBorder();
	if (BorderWrapper)
	{
		return BorderWrapper;
	}
	else
	{
		return Canvas;
	}
}

TObjectPtr<UPanelWidget> FContainerBuilder::GetContainerWidget() const
{
	return Canvas;
}