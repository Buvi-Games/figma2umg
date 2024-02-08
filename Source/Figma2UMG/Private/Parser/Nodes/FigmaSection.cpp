// Fill out your copyright notice in the Description page of Project Settings.


#include "Parser/Nodes/FigmaSection.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void UFigmaSection::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);

	if (JsonObj->HasTypedField<EJson::Object>("devStatus"))
	{
		const TSharedPtr<FJsonObject> DevStatusJson = JsonObj->GetObjectField("devStatus");
		if (DevStatusJson->HasTypedField<EJson::String>("type"))
		{
			DevStatus = JsonObj->GetStringField("type");
		}
	}
}

FVector2D UFigmaSection::GetAbsolutePosition() const
{
	return AbsoluteBoundingBox.GetPosition();
}

FString UFigmaSection::GetCurrentPackagePath() const
{
	FString CurrentPackagePath = Super::GetCurrentPackagePath() + +TEXT("/") + GetNodeName();
	return CurrentPackagePath;
}

void UFigmaSection::ForEach(const IWidgetOwner::FOnEachFunction& Function)
{
	Builder.ForEach(Function);
}

TObjectPtr<UWidget> UFigmaSection::Patch(TObjectPtr<UWidget> WidgetToPatch)
{
	Builder.Border = Cast<UBorder>(WidgetToPatch);
	Builder.Canvas = nullptr;
	if (Builder.Border)
	{
		if (Builder.Border->GetName() != GetUniqueName())
		{
			Builder.Border->Rename(*GetUniqueName());
		}
		Builder.Canvas = Cast<UCanvasPanel>(Builder.Border->GetContent());
	}
	else
	{
		Builder.Border = NewObject<UBorder>(GetAssetOuter(), *GetUniqueName());
	}

	if (!Builder.Canvas)
	{
		Builder.Canvas = NewObject<UCanvasPanel>(GetAssetOuter());
		Builder.Border->SetContent(Builder.Canvas);
	}

	if (Fills.Num() > 0)
	{
		Builder.SetFill(Fills[0]);
	}
	else
	{
		Builder.Border->SetBrushColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
	}

	if (Strokes.Num() > 0)
	{
		Builder.SetStroke(Strokes[0], StrokeWeight, StrokeAlign);
	}

	if (Strokes.Num() > 0)
	{
		Builder.SetStroke(Strokes[0], StrokeWeight, StrokeAlign);
	}

	Builder.SetCorner(0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	return Builder.Border;
}

void UFigmaSection::PostInsert() const
{
	TObjectPtr<UWidget> TopWidget = GetTopWidget();
	if (!TopWidget)
		return;

	IWidgetOwner::PostInsert();

	if (UCanvasPanelSlot* CanvasSlot = TopWidget->Slot ? Cast<UCanvasPanelSlot>(TopWidget->Slot) : nullptr)
	{
		CanvasSlot->SetSize(AbsoluteBoundingBox.GetSize());
	}
}

TObjectPtr<UWidget> UFigmaSection::GetTopWidget() const
{
	return Builder.Border;
}

FVector2D UFigmaSection::GetTopWidgetPosition() const
{
	return GetPosition();
}

TObjectPtr<UPanelWidget> UFigmaSection::GetContainerWidget() const
{
	return Builder.Canvas;
}