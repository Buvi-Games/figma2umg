// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Interfaces/WidgetOwner.h"

#include "Components/BorderSlot.h"
#include "Components/ButtonSlot.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/SizeBoxSlot.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Widget.h"
#include "Components/WrapBoxSlot.h"
#include "Parser/Properties/FigmaLayoutConstraint.h"

void IWidgetOwner::PostInsert() const
{
	PostInsertWidgets(GetTopWidget(), GetContainerWidget());
}

void IWidgetOwner::PostInsertWidgets(TObjectPtr<UWidget> TopWidget, TObjectPtr<UPanelWidget> ContentWidget) const
{
	if (TopWidget)
	{
		SetPosition(TopWidget, GetTopWidgetPosition());
	}
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
		//else if (USizeBoxSlot* SizeBoxSlot = Cast<USizeBoxSlot>(Widget->Slot))
		//{
		//	SizeBoxSlot->SetSize(Size);
		//}
		else if (UHorizontalBoxSlot* HorizontalBoxSlot = Cast<UHorizontalBoxSlot>(Widget->Slot))
		{
			FSlateChildSize ChildSize;
			ChildSize.Value = Size.X;
			ChildSize.SizeRule = SizeToContent ? ESlateSizeRule::Fill : ESlateSizeRule::Automatic;
			HorizontalBoxSlot->SetSize(ChildSize);
		}
		else if (UVerticalBoxSlot* VerticalBoxSlot = Cast<UVerticalBoxSlot>(Widget->Slot))
		{
			FSlateChildSize ChildSize;
			ChildSize.Value = Size.Y;
			ChildSize.SizeRule = SizeToContent ? ESlateSizeRule::Fill : ESlateSizeRule::Automatic;
			VerticalBoxSlot->SetSize(ChildSize);
		}
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
		else if (USizeBoxSlot* SizeBoxSlot = Cast<USizeBoxSlot>(Widget->Slot))
		{
			SizeBoxSlot->SetPadding(Padding);
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
		else if (UButtonSlot* ButtonSlot = Cast<UButtonSlot>(Widget->Slot))
		{
			ButtonSlot->SetPadding(Padding);
		}
	}
}

void IWidgetOwner::SetConstraints(const TObjectPtr<UWidget>& Widget, EFigmaPrimaryAxisAlignItems VerticalAlign, EFigmaCounterAxisAlignItems HorizontalAlign) const
{
	if (Widget && Widget->Slot)
	{
		EHorizontalAlignment HorizontalAlignment = Convert(HorizontalAlign);
		EVerticalAlignment VerticalAlignment = Convert(VerticalAlign);

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
		{
		}
		else if (USizeBoxSlot* SizeBoxSlot = Cast<USizeBoxSlot>(Widget->Slot))
		{
			SizeBoxSlot->SetHorizontalAlignment(HorizontalAlignment);
			SizeBoxSlot->SetVerticalAlignment(VerticalAlignment);
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
		else if (UButtonSlot* ButtonSlot = Cast<UButtonSlot>(Widget->Slot))
		{
			ButtonSlot->SetHorizontalAlignment(HorizontalAlignment);
			ButtonSlot->SetVerticalAlignment(VerticalAlignment);
		}
	}
}

void IWidgetOwner::SetConstraints(TObjectPtr<UWidget> Widget, const FFigmaLayoutConstraint& InConstraints) const
{
	if (Widget && Widget->Slot)
	{
		EHorizontalAlignment HorizontalAlignment = Convert(InConstraints.Horizontal);
		EVerticalAlignment VerticalAlignment = Convert(InConstraints.Vertical);

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
		{
		}
		else if (USizeBoxSlot* SizeBoxSlot = Cast<USizeBoxSlot>(Widget->Slot))
		{
			SizeBoxSlot->SetHorizontalAlignment(HorizontalAlignment);
			SizeBoxSlot->SetVerticalAlignment(VerticalAlignment);
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
		else if (UButtonSlot* ButtonSlot = Cast<UButtonSlot>(Widget->Slot))
		{
			ButtonSlot->SetHorizontalAlignment(HorizontalAlignment);
			ButtonSlot->SetVerticalAlignment(VerticalAlignment);
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
		else if (USizeBoxSlot* SizeBoxSlot = Cast<USizeBoxSlot>(Widget->Slot))
		{
			SizeBoxSlot->SetHorizontalAlignment(HorizontalAlignment);
			SizeBoxSlot->SetVerticalAlignment(VerticalAlignment);
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
		else if (UButtonSlot* ButtonSlot = Cast<UButtonSlot>(Widget->Slot))
		{
			ButtonSlot->SetHorizontalAlignment(HorizontalAlignment);
			ButtonSlot->SetVerticalAlignment(VerticalAlignment);
		}
	}
}

void IWidgetOwner::TryRenameWidget(const FString& InName, TObjectPtr<UWidget> Widget)
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

EHorizontalAlignment IWidgetOwner::Convert(EFigmaLayoutConstraintHorizontal LayoutConstraint) const
{
	switch (LayoutConstraint)
	{
	case EFigmaLayoutConstraintHorizontal::LEFT:
		return HAlign_Left;
	case EFigmaLayoutConstraintHorizontal::RIGHT:
		return HAlign_Right;
	case EFigmaLayoutConstraintHorizontal::CENTER:
		return HAlign_Center;
	case EFigmaLayoutConstraintHorizontal::LEFT_RIGHT:
		return HAlign_Center;
	case EFigmaLayoutConstraintHorizontal::SCALE:
		return HAlign_Fill;
	}

	return HAlign_Center;
}

EHorizontalAlignment IWidgetOwner::Convert(EFigmaCounterAxisAlignItems  LayoutConstraint) const
{
	switch (LayoutConstraint)
	{
	case EFigmaCounterAxisAlignItems::MIN:
		return HAlign_Left;
	case EFigmaCounterAxisAlignItems::CENTER:
		return HAlign_Center;
	case EFigmaCounterAxisAlignItems::MAX:
		return HAlign_Right;

	case EFigmaCounterAxisAlignItems::BASELINE:
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

EVerticalAlignment IWidgetOwner::Convert(EFigmaLayoutConstraintVertical LayoutConstraint) const
{
	switch (LayoutConstraint)
	{
	case EFigmaLayoutConstraintVertical::TOP:
		return VAlign_Top;
	case EFigmaLayoutConstraintVertical::BOTTOM:
		return VAlign_Bottom;
	case EFigmaLayoutConstraintVertical::CENTER:
		return VAlign_Center;
	case EFigmaLayoutConstraintVertical::TOP_BOTTOM:
		return VAlign_Fill;
	case EFigmaLayoutConstraintVertical::SCALE:
		return VAlign_Fill;
	}

	return VAlign_Center;
}

EVerticalAlignment IWidgetOwner::Convert(EFigmaPrimaryAxisAlignItems LayoutConstraint) const
{
	switch (LayoutConstraint)
	{
	case EFigmaPrimaryAxisAlignItems::MIN:
		return VAlign_Top;
	case EFigmaPrimaryAxisAlignItems::CENTER:
		return VAlign_Center;
	case EFigmaPrimaryAxisAlignItems::MAX:
		return VAlign_Bottom;

	case EFigmaPrimaryAxisAlignItems::SPACE_BETWEEN:
		return VAlign_Fill;
	}

	return VAlign_Center;
}
