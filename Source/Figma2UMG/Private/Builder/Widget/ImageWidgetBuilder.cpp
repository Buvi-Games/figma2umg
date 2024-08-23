// Copyright 2024 Buvi Games. All Rights Reserved.


#include "ImageWidgetBuilder.h"

#include "Figma2UMGModule.h"
#include "FigmaImportSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Builder/Asset/Texture2DBuilder.h"
#include "Components/Image.h"
#include "Components/Widget.h"
#include "Parser/Nodes/FigmaGroup.h"
#include "Parser/Nodes/FigmaNode.h"
#include "Parser/Nodes/FigmaSection.h"
#include "Parser/Nodes/Vectors/FigmaEllipse.h"
#include "Parser/Nodes/Vectors/FigmaRectangleVector.h"
#include "Parser/Nodes/Vectors/FigmaVectorNode.h"


void UImageWidgetBuilder::SetTexture2DBuilder(const TObjectPtr<UTexture2DBuilder>& InTexture2DBuilder)
{
	Texture2DBuilder = InTexture2DBuilder;
}

void UImageWidgetBuilder::SetMaterial(const TObjectPtr<UMaterialInterface>& InMaterial, const FLinearColor& InColor)
{
	Material = InMaterial;
	SolidColor = InColor;
}

void UImageWidgetBuilder::SetColor(const FLinearColor& InColor)
{
	HasSolidColor = true;
	SolidColor = InColor;
}

void UImageWidgetBuilder::PatchAndInsertWidget(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UWidget>& WidgetToPatch)
{
	Widget = Cast<UImage>(WidgetToPatch);

	const FString NodeName = Node->GetNodeName();
	const FString WidgetName = Node->GetUniqueName();
	if (Widget)
	{
		UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
		UClass* ClassOverride = Importer ? Importer->GetOverrideClassForNode<UImage>(NodeName) : nullptr;
		if (ClassOverride && Widget->GetClass() != ClassOverride)
		{
			Widget = UFigmaImportSubsystem::NewWidget<UImage>(WidgetBlueprint->WidgetTree, NodeName, WidgetName, ClassOverride);
		}
		UFigmaImportSubsystem::TryRenameWidget(WidgetName, Widget);
	}
	else
	{
		Widget = UFigmaImportSubsystem::NewWidget<UImage>(WidgetBlueprint->WidgetTree, NodeName, WidgetName);
	}

	if (!Texture2DBuilder && !Material)
	{
		UE_LOG_Figma2UMG(Warning, TEXT("[UImageWidgetBuilder::PatchAndInsertWidget] Node<%s> %s didn't set the Texture2DBuilder or UMaterial."), *Node->GetClass()->GetName(), *Node->GetNodeName());
	}

	Insert(WidgetBlueprint->WidgetTree, WidgetToPatch, Widget);

	Setup();
}

bool UImageWidgetBuilder::TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget)
{
	UE_LOG_Figma2UMG(Warning, TEXT("[UImageWidgetBuilder::TryInsertOrReplace] Node %s is an Image and can't insert widgets."), *Node->GetNodeName());
	return false;
}

void UImageWidgetBuilder::SetWidget(const TObjectPtr<UWidget>& InWidget)
{
	Widget = Cast<UImage>(InWidget);
}

TObjectPtr<UWidget> UImageWidgetBuilder::GetWidget() const
{
	return Widget;
}

void UImageWidgetBuilder::ResetWidget()
{
	Widget = nullptr;
}

void UImageWidgetBuilder::Setup() const
{
	SetupFill();

	if (const UFigmaSection* FigmaSection = Cast<UFigmaSection>(Node))
	{
		SetStroke(Widget, FigmaSection->Strokes, FigmaSection->StrokeWeight);
	}
	else if (const UFigmaGroup* FigmaGroup = Cast<UFigmaGroup>(Node))
	{
		SetStroke(Widget, FigmaGroup->Strokes, FigmaGroup->StrokeWeight);

		const FVector4 Corners = FigmaGroup->RectangleCornerRadii.Num() == 4 ? FVector4(FigmaGroup->RectangleCornerRadii[0], FigmaGroup->RectangleCornerRadii[1], FigmaGroup->RectangleCornerRadii[2], FigmaGroup->RectangleCornerRadii[3])
			: FVector4(FigmaGroup->CornerRadius, FigmaGroup->CornerRadius, FigmaGroup->CornerRadius, FigmaGroup->CornerRadius);
		SetCorner(Widget, Corners);
	}
	else if (const UFigmaRectangleVector* FigmaRectangleVector = Cast<UFigmaRectangleVector>(Node))
	{
		SetStroke(Widget, FigmaRectangleVector->Strokes, FigmaRectangleVector->StrokeWeight);

		const FVector4 Corners = FigmaRectangleVector->RectangleCornerRadii.Num() == 4 ? FVector4(FigmaRectangleVector->RectangleCornerRadii[0], FigmaRectangleVector->RectangleCornerRadii[1], FigmaRectangleVector->RectangleCornerRadii[2], FigmaRectangleVector->RectangleCornerRadii[3])
			: FVector4(FigmaRectangleVector->CornerRadius, FigmaRectangleVector->CornerRadius, FigmaRectangleVector->CornerRadius, FigmaRectangleVector->CornerRadius);

		SetCorner(Widget, Corners, !Corners.IsNearlyZero3());
	}
	else if (const UFigmaVectorNode* FigmaVectorNode = Cast<UFigmaVectorNode>(Node))
	{
		SetStroke(Widget, FigmaVectorNode->Strokes, FigmaVectorNode->StrokeWeight);
	}
}

void UImageWidgetBuilder::SetupFill() const
{
	if (const TObjectPtr<UTexture2D>& Texture = Texture2DBuilder ? Texture2DBuilder->GetAsset() : nullptr)
	{
		Widget->SetBrushFromTexture(Texture, false);
		FSlateBrush Brush = Widget->GetBrush();
		Brush.SetImageSize(Node->GetAbsoluteSize());
		if (const UFigmaEllipse* FigmaVectorNode = Cast<UFigmaEllipse>(Node))
		{
			if (FigmaVectorNode->AbsoluteBoundingBox.Height == FigmaVectorNode->AbsoluteBoundingBox.Width)
			{
				Brush.DrawAs = ESlateBrushDrawType::RoundedBox;
			}
		}
		SetBrush(Widget, Brush);
		Widget->SetColorAndOpacity(FLinearColor::White);
	}
	else if (Material)
	{
		Widget->SetBrushFromMaterial(Material);
		FSlateBrush Brush = Widget->GetBrush();
		Brush.SetImageSize(Node->GetAbsoluteSize());
		Brush.TintColor = FLinearColor::White;
		Brush.DrawAs = ESlateBrushDrawType::Box;
		Brush.Margin.Top = 0.5f;
		Brush.Margin.Bottom = 0.5f;
		Brush.Margin.Left = 0.5f;
		Brush.Margin.Right = 0.5f;
		if (const UFigmaEllipse* FigmaVectorNode = Cast<UFigmaEllipse>(Node))
		{
			if (FigmaVectorNode->AbsoluteBoundingBox.Height == FigmaVectorNode->AbsoluteBoundingBox.Width)
			{
				Brush.DrawAs = ESlateBrushDrawType::RoundedBox;
			}
		}
		SetBrush(Widget, Brush);
		Widget->SetColorAndOpacity(SolidColor);
	}
	else
	{
		FSlateBrush Brush = Widget->GetBrush();
		Brush.TintColor = FLinearColor::White;
		Brush.DrawAs = ESlateBrushDrawType::Box;
		Brush.SetImageSize(Node->GetAbsoluteSize());
		if (const UFigmaEllipse* FigmaVectorNode = Cast<UFigmaEllipse>(Node))
		{
			if (FigmaVectorNode->AbsoluteBoundingBox.Height == FigmaVectorNode->AbsoluteBoundingBox.Width)
			{
				Brush.DrawAs = ESlateBrushDrawType::RoundedBox;
			}
		}
		SetBrush(Widget, Brush);
		if (HasSolidColor)
		{
			Widget->SetColorAndOpacity(SolidColor);
		}
		else
		{
			Widget->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f));
		}
	}
};
