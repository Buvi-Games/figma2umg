// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/WidgetOwner.h"

#include "Components/BorderSlot.h"
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

void IWidgetOwner::SetPadding(TObjectPtr<UWidget> Widget, const float PaddingLeft, const float PaddingRight, const float PaddingTop, const float PaddingBottom) const
{
	if (Widget && Widget->Slot)
	{
		FMargin Padding;
		Padding.Left = PaddingLeft;
		Padding.Right = PaddingRight;
		Padding.Top = PaddingTop;
		Padding.Bottom = PaddingBottom;

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
		{
		}
		else if (UBorderSlot* BorderSlot = Cast<UBorderSlot>(Widget->Slot))
		{
			BorderSlot->SetPadding(Padding);
		}
		else if (UHorizontalBoxSlot* HorizontalBoxSlot = Cast<UHorizontalBoxSlot>(Widget->Slot))
		{
			HorizontalBoxSlot->SetPadding(Padding);
		}
		else if (UVerticalBoxSlot* VerticalBoxSlot = Cast<UVerticalBoxSlot>(Widget->Slot))
		{
			VerticalBoxSlot->SetPadding(Padding);
		}
		else if (UWrapBoxSlot* WrapBoxSlot = Cast<UWrapBoxSlot>(Widget->Slot))
		{
			WrapBoxSlot->SetPadding(Padding);
		}
	}
}

void IWidgetOwner::SetAlign(TObjectPtr<UWidget> Widget, EFigmaTextAlignHorizontal TextAlignHorizontal, EFigmaTextAlignVertical TextAlignVertical) const
{
	if (Widget && Widget->Slot)
	{
		EHorizontalAlignment HorizontalAlignment = Convert(TextAlignHorizontal);
		EVerticalAlignment VerticalAlignment = Convert(TextAlignVertical);

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
		{
		}
		else if (UBorderSlot* BorderSlot = Cast<UBorderSlot>(Widget->Slot))
		{
			BorderSlot->SetHorizontalAlignment(HorizontalAlignment);
			BorderSlot->SetVerticalAlignment(VerticalAlignment);
		}
		else if (UHorizontalBoxSlot* HorizontalBoxSlot = Cast<UHorizontalBoxSlot>(Widget->Slot))
		{
			HorizontalBoxSlot->SetHorizontalAlignment(HorizontalAlignment);
			HorizontalBoxSlot->SetVerticalAlignment(VerticalAlignment);
		}
		else if (UVerticalBoxSlot* VerticalBoxSlot = Cast<UVerticalBoxSlot>(Widget->Slot))
		{
			VerticalBoxSlot->SetHorizontalAlignment(HorizontalAlignment);
			VerticalBoxSlot->SetVerticalAlignment(VerticalAlignment);
		}
		else if (UWrapBoxSlot* WrapBoxSlot = Cast<UWrapBoxSlot>(Widget->Slot))
		{
			WrapBoxSlot->SetHorizontalAlignment(HorizontalAlignment);
			WrapBoxSlot->SetVerticalAlignment(VerticalAlignment);
		}
	}
}

void IWidgetOwner::TryRenameWidget(const FString& InName, TObjectPtr<UWidget> Widget) const
{
	if (!Widget)
		return;

	if (Widget->GetName().Contains(InName, ESearchCase::IgnoreCase))
		return;

	const FString UniqueName = MakeUniqueObjectName(Widget->GetOuter(), Widget->GetClass(), *InName).ToString();
	Widget->Rename(*UniqueName);
}

EHorizontalAlignment IWidgetOwner::Convert(EFigmaTextAlignHorizontal TextAlignHorizontal) const
{
	switch (TextAlignHorizontal)
	{
	case EFigmaTextAlignHorizontal::LEFT:
		return HAlign_Left;
	case EFigmaTextAlignHorizontal::CENTER:
		return HAlign_Center;
	case EFigmaTextAlignHorizontal::RIGHT:
		return HAlign_Right;
	case EFigmaTextAlignHorizontal::JUSTIFIED:
		return HAlign_Fill;
	}
	return HAlign_Center;
}

EVerticalAlignment IWidgetOwner::Convert(EFigmaTextAlignVertical TextAlignVertical) const
{
	switch (TextAlignVertical)
	{
	case EFigmaTextAlignVertical::TOP:
		return VAlign_Top;
	case EFigmaTextAlignVertical::CENTER:
		return VAlign_Center;
	case EFigmaTextAlignVertical::BOTTOM:
		return VAlign_Bottom;
	}

	return VAlign_Center;
}