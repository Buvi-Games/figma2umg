// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/Nodes/FigmaSection.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

FVector2D UFigmaSection::GetTopWidgetPosition() const
{
	return GetPosition();
}

FVector2D UFigmaSection::GetSize() const
{
	return AbsoluteBoundingBox.GetSize();
}

FLinearColor UFigmaSection::GetBrushColor() const
{
	return Fills.Num() > 0 ? Fills[0].Color.GetLinearColor() : FLinearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

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

TObjectPtr<UWidget> UFigmaSection::Patch(TObjectPtr<UWidget> WidgetToPatch)
{
	return AddOrPatchContent(Cast<UBorder>(WidgetToPatch), GetAssetOuter(), GetUniqueName());
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
