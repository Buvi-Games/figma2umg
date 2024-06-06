// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/FigmaGroup.h"

#include "FigmaComponent.h"
#include "Builder/ButtonBuilder.h"
#include "Builder/Widget/BorderWidgetBuilder.h"
#include "Builder/Widget/CanvasBuilder.h"
#include "Builder/Widget/HBoxBuilder.h"
#include "Builder/Widget/PanelWidgetBuilder.h"
#include "Builder/Widget/SizeBoxWidgetBuilder.h"
#include "Builder/Widget/VBoxBuilder.h"
#include "Builder/Widget/WBoxBuilder.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/SizeBoxSlot.h"
#include "Components/Spacer.h"
#include "Components/WrapBox.h"


void UFigmaGroup::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);

	PostSerializeProperty(JsonObj, "fills", Fills);
	PostSerializeProperty(JsonObj, "strokes", Strokes);
}

TScriptInterface<IWidgetBuilder> UFigmaGroup::CreateWidgetBuilders() const
{
	USizeBoxWidgetBuilder* SizeBoxWidgetBuilder = nullptr;
	UBorderWidgetBuilder* BorderWidgetBuilder = nullptr;
	UPanelWidgetBuilder* PanelWidgetBuilder = nullptr;
	if (LayoutSizingHorizontal == EFigmaLayoutSizing::FIXED || LayoutSizingVertical == EFigmaLayoutSizing::FIXED)
	{
		SizeBoxWidgetBuilder = NewObject<USizeBoxWidgetBuilder>();
		SizeBoxWidgetBuilder->SetNode(this);
	}

	bool RequireBorder = false;
	for (int i = 0; i < Fills.Num() && !RequireBorder; i++)
	{
		if (Fills[i].Visible)
			RequireBorder = true;
	}
	for (int i = 0; i < Strokes.Num() && !RequireBorder; i++)
	{
		if (Strokes[i].Visible)
			RequireBorder = true;
	}

	if (RequireBorder)
	{
		BorderWidgetBuilder = NewObject<UBorderWidgetBuilder>();
		BorderWidgetBuilder->SetNode(this);
		if (SizeBoxWidgetBuilder)
		{
			SizeBoxWidgetBuilder->SetChild(BorderWidgetBuilder);
		}
	}	

	switch (LayoutMode)
	{
	case EFigmaLayoutMode::NONE:
	{
		PanelWidgetBuilder = NewObject<UCanvasBuilder>();
	}
	break;
	case EFigmaLayoutMode::HORIZONTAL:
	{
		if (LayoutWrap == EFigmaLayoutWrap::NO_WRAP)
		{
			PanelWidgetBuilder = NewObject<UHBoxBuilder>();
		}
		else
		{
			PanelWidgetBuilder = NewObject<UWBoxBuilder>();
		}
	}
	break;
	case EFigmaLayoutMode::VERTICAL:
	{
		if (LayoutWrap == EFigmaLayoutWrap::NO_WRAP)
		{
			PanelWidgetBuilder = NewObject<UVBoxBuilder>();
		}
		else
		{
			PanelWidgetBuilder = NewObject<UWBoxBuilder>();
		}
	}
	break;
	}

	PanelWidgetBuilder->SetNode(this);
	for (const UFigmaNode* Child : Children)
	{
		if (TScriptInterface<IWidgetBuilder> SubBuilder = Child->CreateWidgetBuilders())
		{
			PanelWidgetBuilder->AddChild(SubBuilder);
		}
	}


	if (BorderWidgetBuilder)
	{
		BorderWidgetBuilder->SetChild(PanelWidgetBuilder);
	}
	else if(SizeBoxWidgetBuilder)
	{
		SizeBoxWidgetBuilder->SetChild(PanelWidgetBuilder);
	}

	if (SizeBoxWidgetBuilder)
	{
		return SizeBoxWidgetBuilder;
	}

	if(BorderWidgetBuilder)
	{
		return BorderWidgetBuilder;
	}

	return PanelWidgetBuilder;
}

FVector2D UFigmaGroup::GetAbsolutePosition() const
{
	return AbsoluteBoundingBox.GetPosition();
}

TObjectPtr<UWidget> UFigmaGroup::PatchPreInsertWidget(TObjectPtr<UWidget> WidgetToPatch)
{
	TObjectPtr<UWidget> WidgetPatched = Super::PatchPreInsertWidget(WidgetToPatch);

	UPanelWidget* ParentWidget = Cast<UPanelWidget>(WidgetPatched);
	if (IWidgetOwner* WidgetOwner = Cast<IWidgetOwner>(this))
	{
		ParentWidget = WidgetOwner->GetContainerWidget();
	}

	if (ParentWidget)
	{
		FixSpacers(ParentWidget);
	}

	return WidgetPatched;
}

void UFigmaGroup::ForEach(const IWidgetOwner::FOnEachFunction& Function)
{
	Builder.ForEach(Function);
}

TObjectPtr<UWidget> UFigmaGroup::Patch(TObjectPtr<UWidget> WidgetToPatch)
{
	FVector4 Corners = RectangleCornerRadii.Num() == 4 ? FVector4(RectangleCornerRadii[0], RectangleCornerRadii[1], RectangleCornerRadii[2], RectangleCornerRadii[3]) : FVector4(CornerRadius, CornerRadius, CornerRadius, CornerRadius);
	Builder.SetupBorder(Fills, Strokes, StrokeWeight, StrokeAlign, Corners, CornerSmoothing);
	Builder.SetLayout(LayoutMode, LayoutWrap);
	Builder.SetLayoutSize(LayoutSizingHorizontal, AbsoluteBoundingBox.Width, LayoutSizingVertical, AbsoluteBoundingBox.Height);
	return Builder.Patch(WidgetToPatch, GetAssetOuter(), GetUniqueName());
}

void UFigmaGroup::SetupBrush(FSlateBrush& Brush) const
{
	FVector4 Corners = RectangleCornerRadii.Num() == 4 ? FVector4(RectangleCornerRadii[0], RectangleCornerRadii[1], RectangleCornerRadii[2], RectangleCornerRadii[3]) : FVector4(CornerRadius, CornerRadius, CornerRadius, CornerRadius);
	Builder.SetupBrush(Brush, Fills, Strokes, StrokeWeight, StrokeAlign, Corners, CornerSmoothing);
}

void UFigmaGroup::SetupLayout(FContainerBuilder& ContainerBuilder)
{
	ContainerBuilder.SetLayout(LayoutMode, LayoutWrap);
}

void UFigmaGroup::SetupWidget(TObjectPtr<UWidget> Widget)
{
	if (Widget)
	{
		UE_LOG_Figma2UMG(Display, TEXT("[SetupWidget] UFigmaGroup %s received a UWidget %s of type %s."), *GetNodeName(), *Widget->GetName(), *Widget->GetClass()->GetDisplayNameText().ToString());
	}

	Builder.SetupWidget(Widget);

	// SizeBoxSlot are not ready during PostInsertWidgets
	TObjectPtr<UWidget> ContentWidget = GetTopWidget();
	USizeBoxSlot* SizeBoxSlot = ContentWidget ? Cast<USizeBoxSlot>(ContentWidget->Slot) : nullptr;
	if (SizeBoxSlot)
	{
		SetPadding(ContentWidget, PaddingLeft, PaddingRight, PaddingTop, PaddingBottom);
		SetConstraints(ContentWidget, PrimaryAxisAlignItems, CounterAxisAlignItems);
	}
}

void UFigmaGroup::PostInsertWidgets(TObjectPtr<UWidget> TopWidget, TObjectPtr<UPanelWidget> ContentWidget) const
{
	IWidgetOwner::PostInsertWidgets(TopWidget, ContentWidget);
	if (TopWidget)
	{
		const bool SizeToContent = LayoutSizingHorizontal == EFigmaLayoutSizing::FILL || LayoutSizingVertical == EFigmaLayoutSizing::FILL;
		SetSize(TopWidget, AbsoluteBoundingBox.GetSize(), SizeToContent);
	}

	if (ContentWidget)
	{
		if(TopWidget->IsA<UButton>())
		{
			SetPadding(ContentWidget, 0.0f, 0.0f, 0.0f, 0.0f);
		}
		else
		{
			SetPadding(ContentWidget, PaddingLeft, PaddingRight, PaddingTop, PaddingBottom);
			SetConstraints(ContentWidget, PrimaryAxisAlignItems, CounterAxisAlignItems);
		}
	}
}

void UFigmaGroup::Reset()
{
	Builder.Reset();
}

TObjectPtr<UWidget> UFigmaGroup::GetTopWidget() const
{
	return Builder.GetTopWidget();
}

FVector2D UFigmaGroup::GetTopWidgetPosition() const
{
	return GetPosition();
}

TObjectPtr<UPanelWidget> UFigmaGroup::GetContainerWidget() const
{
	return Builder.GetContainerWidget();
}

void UFigmaGroup::PatchBinds(TObjectPtr<UWidgetBlueprint> WidgetBp) const
{
	if (WidgetBp == nullptr)
		return;

	TObjectPtr<UWidget> Widget = GetTopWidget();
	ProcessComponentPropertyReferences(WidgetBp, Widget);

	for (UFigmaNode* Child : Children)
	{
		IWidgetOwner* WidgetOwner = Cast<IWidgetOwner>(Child);
		if (WidgetOwner)
		{
			WidgetOwner->PatchBinds(WidgetBp);
		}
	}
}

FMargin UFigmaGroup::GetPadding() const
{
	FMargin Padding;
	Padding.Left = PaddingLeft;
	Padding.Right = PaddingRight;
	Padding.Top = PaddingTop;
	Padding.Bottom = PaddingBottom;

	return Padding;
}

void UFigmaGroup::FixSpacers(const TObjectPtr<UPanelWidget>& PanelWidget) const
{
	if (!PanelWidget)
		return;

	if (PanelWidget->IsA<UCanvasPanel>() || PanelWidget->IsA<UWrapBox>())
	{
		for (int i = 0; i < PanelWidget->GetChildrenCount(); i++)
		{
			UWidget* Widget = PanelWidget->GetChildAt(i);
			if (!Widget || Widget->IsA<USpacer>())
			{
				PanelWidget->RemoveChildAt(i);
				i--;
			}
		}
		if (UWrapBox* WrapBox = Cast<UWrapBox>(PanelWidget))
		{
			WrapBox->SetInnerSlotPadding(FVector2D(ItemSpacing, CounterAxisSpacing));
		}
	}
	else
	{
		for (int i = 0; i < PanelWidget->GetChildrenCount(); i++)
		{
			UWidget* Widget = PanelWidget->GetChildAt(i);
			const bool ShouldBeSpacer = (((i + 1) % 2) == 0);
			const bool IsSpacer = Widget && Widget->IsA<USpacer>();
			if (!Widget || (IsSpacer && !ShouldBeSpacer))
			{
				PanelWidget->RemoveChildAt(i);
				i--;
			}
			else if (ShouldBeSpacer && !IsSpacer)
			{
				USpacer* Spacer = NewObject<USpacer>(PanelWidget->GetOuter());
				Spacer->SetSize(FVector2D(ItemSpacing, ItemSpacing));
				PanelWidget->InsertChildAt(i, Spacer);
			}
			else if (ShouldBeSpacer && IsSpacer)
			{
				USpacer* Spacer = Cast<USpacer>(Widget);
				Spacer->SetSize(FVector2D(ItemSpacing, ItemSpacing));
			}
		}
	}

}