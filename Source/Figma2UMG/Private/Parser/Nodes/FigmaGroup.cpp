// Fill out your copyright notice in the Description page of Project Settings.


#include "Parser/Nodes/FigmaGroup.h"

#include "FigmaComponent.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"


FVector2D UFigmaGroup::GetAbsolutePosition() const
{
	return AbsoluteBoundingBox.GetPosition();
}

void UFigmaGroup::ForEach(const IWidgetOwner::FOnEachFunction& Function)
{
	Builder.ForEach(Function);
}

TObjectPtr<UWidget> UFigmaGroup::Patch(TObjectPtr<UWidget> WidgetToPatch)
{
	FVector4 corners = RectangleCornerRadii.Num() == 4 ? FVector4(RectangleCornerRadii[0], RectangleCornerRadii[1], RectangleCornerRadii[2], RectangleCornerRadii[3]) : FVector4(CornerRadius, CornerRadius, CornerRadius, CornerRadius);
	Builder.SetupBorder(Fills, Strokes, StrokeWeight, StrokeAlign, corners, CornerSmoothing);
	Builder.SetLayout(LayoutMode, LayoutWrap);
	return Builder.Patch(WidgetToPatch, GetAssetOuter(), GetUniqueName());
}

void UFigmaGroup::SetupWidget(TObjectPtr<UWidget> Widget)
{
	if (Widget)
	{
		UE_LOG_Figma2UMG(Display, TEXT("[SetupWidget] UFigmaGroup %s received a UWidget %s of type %s."), *GetNodeName(), *Widget->GetName(), *Widget->GetClass()->GetDisplayNameText().ToString());
	}

	Builder.SetupWidget(Widget);
}

void UFigmaGroup::PostInsert() const
{
	TObjectPtr<UWidget> TopWidget = GetTopWidget();
	if (!TopWidget)
		return;

	IWidgetOwner::PostInsert();

	SetSize(TopWidget, AbsoluteBoundingBox.GetSize());
	SetPadding(GetContainerWidget(), PaddingLeft, PaddingRight, PaddingTop, PaddingBottom);
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
		if (Child->IsA<UFigmaComponent>())
			continue;

		IWidgetOwner* WidgetOwner = Cast<IWidgetOwner>(Child);
		if (WidgetOwner)
		{
			WidgetOwner->PatchBinds(WidgetBp);
		}
	}
}
