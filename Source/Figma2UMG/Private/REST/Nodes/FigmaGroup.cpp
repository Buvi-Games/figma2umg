// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/Nodes/FigmaGroup.h"


FVector2D UFigmaGroup::GetPosition() const
{
	return UFigmaNode::GetPosition();
}

FVector2D UFigmaGroup::GetAbsolutePosition() const
{
	return AbsoluteBoundingBox.GetPosition();
}

FVector2D UFigmaGroup::GetSize() const
{
	return AbsoluteBoundingBox.GetSize();
}

void UFigmaGroup::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);

	SerializeArray(Children, JsonObj, "Children");
}

TObjectPtr<UWidget> UFigmaGroup::AddOrPathToWidgetImp(TObjectPtr<UWidget> WidgetToPatch)
{
	return AddOrPathContent(Cast<UBorder>(WidgetToPatch), GetAssetOuter(), GetUniqueName());
}

void UFigmaGroup::PostInsert(UWidget* Widget) const
{
	Super::PostInsert(Widget);
	PostInsertContent(Fills.Num() > 0 ? Fills[0].Color.GetLinearColor() : FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
	AddOrPathChildren(Canvas, Children);
}
