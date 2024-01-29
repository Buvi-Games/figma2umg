// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/Nodes/FigmaSection.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void UFigmaSection::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);

	//Todo: Translate AbsoluteBoundingBox to LocalBoundingBox.

	if (JsonObj->HasTypedField<EJson::Object>("devStatus"))
	{
		const TSharedPtr<FJsonObject> DevStatusJson = JsonObj->GetObjectField("devStatus");
		if (DevStatusJson->HasTypedField<EJson::String>("type"))
		{
			DevStatus = JsonObj->GetStringField("type");
		}
	}

	SerializeArray(Children, JsonObj, "Children");
}

TObjectPtr<UWidget> UFigmaSection::AddOrPathToWidget(TObjectPtr<UWidgetTree> Outer, TObjectPtr<UWidget> WidgetToPatch) const
{
	UBorder* Border = WidgetToPatch ? Cast<UBorder>(WidgetToPatch) : nullptr;
	UCanvasPanel* Canvas = nullptr;
	if (Border)
	{
		if (Border->GetName() != GetUniqueName())
		{
			Border->Rename(*GetUniqueName());
		}
		Canvas = Cast<UCanvasPanel>(Border->GetContent());
	}
	else
	{
		Border = NewObject<UBorder>(Outer, *GetUniqueName());
	}

	if (!Canvas)
	{
		Canvas = NewObject<UCanvasPanel>(Outer);
		Border->SetContent(Canvas);
	}


	return Border;
}

void UFigmaSection::PostInsert(UWidget* Widget) const
{
	Super::PostInsert(Widget);

	UCanvasPanelSlot* CanvasSlot = Widget->Slot ? Cast<UCanvasPanelSlot>(Widget->Slot) : nullptr;
	if (CanvasSlot)
	{
		//Todo: Make it relative to parent.
		//CanvasSlot->SetPosition(AbsoluteBoundingBox.GetPosition());
		CanvasSlot->SetPosition(GetPosition());
		CanvasSlot->SetSize(AbsoluteBoundingBox.GetSize());
	}

	UBorder* Border = Widget ? Cast<UBorder>(Widget) : nullptr;
	if (Border)
	{
		if (Fills.Num() > 0)
		{
			Border->SetBrushColor(Fills[0].Color.GetLinearColor());
		}
;
		if(UCanvasPanel * Canvas = Cast<UCanvasPanel>(Border->GetContent()))
		{
			AddOrPathChildren(Canvas, Children);
		}
	}
}

FVector2D UFigmaSection::GetAbsolutePosition() const
{
	return AbsoluteBoundingBox.GetPosition();
}
