// Fill out your copyright notice in the Description page of Project Settings.


#include "Builder/SizeBoxBuilder.h"

#include "BorderBuilder.h"
#include "Components/Widget.h"
#include "Figma2UMGModule.h"
#include "Components/Border.h"
#include "Components/SizeBox.h"
#include "Parser/Properties/FigmaPaint.h"
#include "UObject/ObjectPtr.h"

#define ALWAYS_BORDER false

void FSizeBoxBuilder::ForEach(const IWidgetOwner::FOnEachFunction& Function)
{
	if (!Function.IsBound())
		return;

	if (SizeBox)
	{
		Function.ExecuteIfBound(*SizeBox);
	}

	BorderBuilder.ForEach(Function);
}

void FSizeBoxBuilder::SetupBorder(const TArray<FFigmaPaint>& Fills, const TArray<FFigmaPaint>& Strokes, float InStrokeWeight, const EFigmaStrokeAlign& InStrokeAlign, const FVector4& InCornerRadii, const float InCornerSmoothing)
{
	BorderBuilder.SetupBorder(Fills, Strokes, InStrokeWeight, InStrokeAlign, InCornerRadii, InCornerSmoothing);
}

TObjectPtr<UWidget> FSizeBoxBuilder::GetTopWidget() const
{
	if (SizeBox)
	{
		return SizeBox;
	}
	else
	{
		return BorderBuilder.GetTopWidget();
	}
}

TObjectPtr<UPanelWidget> FSizeBoxBuilder::GetContainerWidget() const
{
	return BorderBuilder.GetContainerWidget();
}

TObjectPtr<UWidget> FSizeBoxBuilder::Patch(TObjectPtr<UWidget> WidgetToPatch, UObject* AssetOuter, const FString& WidgetName)
{
	if (!AssetOuter)
	{
		UE_LOG_Figma2UMG(Error, TEXT("[FSizeBoxBuilder::SetupWiPatchdget] AssetOuter is nullptr."));
		return nullptr;
	}

	const bool RequireSizeBox = LayoutSizingHorizontal == EFigmaLayoutSizing::FIXED || LayoutSizingVertical == EFigmaLayoutSizing::FIXED;
	if(RequireSizeBox)
	{
		SizeBox = Cast<USizeBox>(WidgetToPatch);
		if (SizeBox)
		{
			IWidgetOwner::TryRenameWidget(WidgetName, SizeBox);
		}
		else
		{
			SizeBox = IWidgetOwner::NewWidget<USizeBox>(AssetOuter, *WidgetName);

			if (WidgetToPatch)
			{
				SizeBox->SetContent(WidgetToPatch);
			}
		}

		if (LayoutSizingHorizontal == EFigmaLayoutSizing::FIXED)
		{
			SizeBox->SetWidthOverride(FixedWidth);
		}

		if (LayoutSizingVertical == EFigmaLayoutSizing::FIXED)
		{
			SizeBox->SetHeightOverride(FixedHeight);
		}
	}
	else
	{
		SizeBox = nullptr;
	}

	if (SizeBox)
	{
		BorderBuilder.Patch(SizeBox->GetContent(), AssetOuter, "");
		SizeBox->SetContent(BorderBuilder.GetTopWidget());
	}
	else
	{
		BorderBuilder.Patch(WidgetToPatch, AssetOuter, WidgetName);
	}

	return GetTopWidget();
}

void FSizeBoxBuilder::SetupWidget(TObjectPtr<UWidget> Widget)
{
	SizeBox = Cast<USizeBox>(Widget);
	if (SizeBox)
	{
		BorderBuilder.SetupWidget(SizeBox->GetContent());
	}
	else
	{
		BorderBuilder.SetupWidget(Widget);
		const bool RequireSizeBox = LayoutSizingHorizontal == EFigmaLayoutSizing::FIXED || LayoutSizingVertical == EFigmaLayoutSizing::FIXED;
		if (RequireSizeBox)
		{
			if(Widget)
			{
				UE_LOG_Figma2UMG(Error, TEXT("[FSizeBoxBuilder::SetupWidget] Fail to setup USizeBox from UWidget %s of type %s."), *Widget->GetName(), *Widget->GetClass()->GetDisplayNameText().ToString());
			}
			else
			{
				UE_LOG_Figma2UMG(Warning, TEXT("[FSizeBoxBuilder::SetupWidget] Fail to setup USizeBox from a null UWidget."));
			}
		}
	}


}


void FSizeBoxBuilder::SetupBrush(FSlateBrush& Brush, const TArray<FFigmaPaint>& Fills, const TArray<FFigmaPaint>& Strokes, float InStrokeWeight, EFigmaStrokeAlign InStrokeAlign, const FVector4& InCornerRadii, float InCornerSmoothing) const
{
	BorderBuilder.SetupBrush(Brush, Fills, Strokes, InStrokeWeight, InStrokeAlign, InCornerRadii, InCornerSmoothing);
}

void FSizeBoxBuilder::SetLayout(EFigmaLayoutMode InLayoutMode, EFigmaLayoutWrap InLayoutWrap)
{
	BorderBuilder.SetLayout(InLayoutMode, InLayoutWrap);
}

void FSizeBoxBuilder::SetLayoutSize(EFigmaLayoutSizing InLayoutSizingHorizontal, float InWidth, EFigmaLayoutSizing InLayoutSizingVertical, float InHeight)
{
	LayoutSizingHorizontal = InLayoutSizingHorizontal;
	LayoutSizingVertical = InLayoutSizingVertical;
	FixedWidth = InWidth;
	FixedHeight = InHeight;
}

void FSizeBoxBuilder::Reset()
{
	SizeBox = nullptr;
	BorderBuilder.Reset();
}
