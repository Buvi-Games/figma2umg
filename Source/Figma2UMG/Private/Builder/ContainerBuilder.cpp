// Fill out your copyright notice in the Description page of Project Settings.


#include "Builder/ContainerBuilder.h"

#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Components/WrapBox.h"

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
	
	switch (LayoutMode)
	{
	case EFigmaLayoutMode::NONE:
	{
		Patch<UCanvasPanel>(WidgetToPatch, AssetOuter, WidgetName);
	}
	break;
	case EFigmaLayoutMode::HORIZONTAL:
	{
		if(LayoutWrap == EFigmaLayoutWrap::NO_WRAP)
		{
			Patch<UHorizontalBox>(WidgetToPatch, AssetOuter, WidgetName);
		}
		else
		{
			TObjectPtr<UWrapBox> WrapBox = Patch<UWrapBox>(WidgetToPatch, AssetOuter, WidgetName);
			WrapBox->SetOrientation(EOrientation::Orient_Horizontal);
		}
	}
	break;
	case EFigmaLayoutMode::VERTICAL:
	{
		if (LayoutWrap == EFigmaLayoutWrap::NO_WRAP)
		{
			Patch<UVerticalBox>(WidgetToPatch, AssetOuter, WidgetName);
		}
		else
		{
			TObjectPtr<UWrapBox> WrapBox = Patch<UWrapBox>(WidgetToPatch, AssetOuter, WidgetName);
			WrapBox->SetOrientation(EOrientation::Orient_Vertical);
		}
	}
	break;
	}

	return GetTopWidget();

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