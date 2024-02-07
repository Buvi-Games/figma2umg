// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/Nodes/FigmaGroup.h"


FVector2D UFigmaGroup::GetTopWidgetPosition() const
{
	return GetPosition();
}

FVector2D UFigmaGroup::GetAbsolutePosition() const
{
	return AbsoluteBoundingBox.GetPosition();
}

FVector2D UFigmaGroup::GetSize() const
{
	return AbsoluteBoundingBox.GetSize();
}

FLinearColor UFigmaGroup::GetBrushColor() const
{
	return Fills.Num() > 0 ? Fills[0].Color.GetLinearColor() : FLinearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

TObjectPtr<UWidget> UFigmaGroup::Patch(TObjectPtr<UWidget> WidgetToPatch)
{
	return AddOrPatchContent(Cast<UBorder>(WidgetToPatch), GetAssetOuter(), GetUniqueName());
}
