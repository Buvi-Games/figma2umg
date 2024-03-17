// Fill out your copyright notice in the Description page of Project Settings.


#include "Builder/BorderBuilder.h"

#include "Components/Widget.h"
#include "Figma2UMGModule.h"
#include "Components/Border.h"
#include "Parser/Properties/FigmaPaint.h"
#include "UObject/ObjectPtr.h"

#define ALWAYS_BORDER false

void FBorderBuilder::ForEach(const IWidgetOwner::FOnEachFunction& Function)
{
	if (!Function.IsBound())
		return;

	if (Border)
	{
		Function.ExecuteIfBound(*Border);
	}

	ContainerBuilder.ForEach(Function);
}

void FBorderBuilder::SetupBorder(const TArray<FFigmaPaint>& Fills, const TArray<FFigmaPaint>& Strokes, float InStrokeWeight, const EFigmaStrokeAlign& InStrokeAlign, const FVector4& InCornerRadii, const float InCornerSmoothing)
{
	if (!Fills.IsEmpty())
	{
		Fill = &Fills[0];
	}

	if (!Strokes.IsEmpty())
	{
		Stroke = &Strokes[0];
	}
	StrokeWeight = InStrokeWeight;

	CornerRadii = InCornerRadii;
	CornerSmoothing = InCornerSmoothing;
}

TObjectPtr<UWidget> FBorderBuilder::GetTopWidget() const
{
	if (Border)
	{
		return Border;
	}
	else
	{
		return ContainerBuilder.GetContainerWidget();
	}
}

TObjectPtr<UPanelWidget> FBorderBuilder::GetContainerWidget() const
{
	return ContainerBuilder.GetContainerWidget();
}

TObjectPtr<UWidget> FBorderBuilder::Patch(TObjectPtr<UWidget> WidgetToPatch, UObject* AssetOuter, const FString& WidgetName)
{
	const bool RequireBorder = ((Fill && Fill->Visible) || (Stroke && Stroke->Visible));
	if (RequireBorder || ALWAYS_BORDER)
	{
		Border = Cast<UBorder>(WidgetToPatch);
		if (Border)
		{
			if (Border->GetName() != WidgetName)
			{
				if (StaticFindObject(nullptr, Border->GetOuter(), *WidgetName, true))
				{
					UE_LOG_Figma2UMG(Error, TEXT("Failt to rename %s to %s. Name already exists."), *Border->GetName(), *WidgetName);
				}
				else
				{
					Border->Rename(*WidgetName);
				}
			}
		}
		else
		{
			if(WidgetToPatch && WidgetToPatch->GetName() == WidgetName)
			{
				FString OldName = WidgetName + "_OLD";
				WidgetToPatch->Rename(*OldName);
			}
			Border = NewObject<UBorder>(AssetOuter, *WidgetName);
			if (WidgetToPatch)
			{
				Border->SetContent(WidgetToPatch);
			}
		}

		if (Border)
		{
			FSlateBrush Brush = Border->Background;
			Brush.DrawAs = RequireBorder ? ESlateBrushDrawType::RoundedBox : ESlateBrushDrawType::NoDrawType;
			Border->SetBrush(Brush);
		}

		SetFill();
		SetStroke();
		SetCorner();
	}
	else
	{
		Border = nullptr;
	}

	if (Border)
	{
		ContainerBuilder.Patch(Border->GetContent(), AssetOuter, "");
		Border->SetContent(ContainerBuilder.GetContainerWidget());
	}
	else
	{
		ContainerBuilder.Patch(WidgetToPatch, AssetOuter, WidgetName);
	}

	return GetTopWidget();
}

void FBorderBuilder::SetupWidget(TObjectPtr<UWidget> Widget)
{
	Border = Cast<UBorder>(Widget);
	if (Border)
	{
		ContainerBuilder.SetupWidget(Border->GetContent());
	}
	else
	{
		ContainerBuilder.SetupWidget(Widget);
		const bool RequireBorder = ((Fill && Fill->Visible) || (Stroke && Stroke->Visible));
		if (RequireBorder || ALWAYS_BORDER)
		{
			if(Widget)
			{
				UE_LOG_Figma2UMG(Error, TEXT("[FBorderBuilder::SetupWidget] Fail to setup UBorder from UWidget %s of type %s."), *Widget->GetName(), *Widget->GetClass()->GetDisplayNameText().ToString());
			}
			else
			{
				UE_LOG_Figma2UMG(Warning, TEXT("[FBorderBuilder::SetupWidget] Fail to setup UBorder from a null UWidget."));
			}
		}
	}


}


void FBorderBuilder::SetupBrush(FSlateBrush& Brush, const TArray<FFigmaPaint>& Fills, const TArray<FFigmaPaint>& Strokes, float InStrokeWeight, EFigmaStrokeAlign InStrokeAlign, const FVector4& InCornerRadii, float InCornerSmoothing) const
{
	if (!Fills.IsEmpty() && Fills[0].Visible)
	{
		Brush.TintColor = Fills[0].Color.GetLinearColor();
	}
	else
	{
		Brush.TintColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.0f);
	}

	if (!Strokes.IsEmpty())
	{
		Brush.OutlineSettings.Color = Strokes[0].Color.GetLinearColor();
		Brush.OutlineSettings.Width = InStrokeWeight;
	}
	else
	{
		Brush.OutlineSettings.Color = FLinearColor(1.0f, 1.0f, 1.0f, 0.0f);
		Brush.OutlineSettings.Width = 0.0f;

	}

	Brush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
	Brush.OutlineSettings.CornerRadii = InCornerRadii;
}

void FBorderBuilder::SetLayout(EFigmaLayoutMode InLayoutMode, EFigmaLayoutWrap InLayoutWrap)
{
	ContainerBuilder.SetLayout(InLayoutMode, InLayoutWrap);
}

void FBorderBuilder::SetFill() const
{
	if (Border)
	{
		if (Fill && Fill->Visible)
		{
			Border->SetBrushColor(Fill->Color.GetLinearColor());
		}
		else
		{
			Border->SetBrushColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
		}
	}
}

void FBorderBuilder::SetStroke() const
{
	if (Border)
	{
		if(Stroke)
		{
			FSlateBrush Brush = Border->Background;
			Brush.OutlineSettings.Color = Stroke->Color.GetLinearColor();
			Brush.OutlineSettings.Width = StrokeWeight;
			Border->SetBrush(Brush);
		}
		else
		{
			FSlateBrush Brush = Border->Background;
			Brush.OutlineSettings.Color = FLinearColor(1.0f, 1.0f, 1.0f, 0.0f);
			Brush.OutlineSettings.Width = 0.0f;
			Border->SetBrush(Brush);
			
		}
	}
}

void FBorderBuilder::SetCorner() const
{
	if (Border && Border->Background.GetDrawType() == ESlateBrushDrawType::RoundedBox)
	{
		FSlateBrush Brush = Border->Background;
		Brush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
		Brush.OutlineSettings.CornerRadii = CornerRadii;		
		Border->SetBrush(Brush);
	}
}

void FBorderBuilder::Reset()
{
	Border = nullptr;
	ContainerBuilder.Reset();
}
