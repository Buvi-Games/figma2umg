// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/Nodes/FigmaSection.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

FVector2D UFigmaSection::GetPosition() const
{
	return UFigmaNode::GetPosition();
}

FVector2D UFigmaSection::GetSize() const
{
	return AbsoluteBoundingBox.GetSize();
}

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

TObjectPtr<UWidget> UFigmaSection::AddOrPathToWidgetImp(TObjectPtr<UWidget> WidgetToPatch)
{
	return AddOrPathContent(Cast<UBorder>(WidgetToPatch), GetAssetOuter(), GetUniqueName());
}

void UFigmaSection::PostInsert(UWidget* Widget) const
{
	Super::PostInsert(Widget);
	PostInsertContent(Fills.Num() > 0 ? Fills[0].Color.GetLinearColor() : FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
	AddOrPathChildren(Canvas, Children);
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
