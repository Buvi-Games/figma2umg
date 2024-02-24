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
	Builder.SetupBorder(Fills, Strokes, StrokeWeight, StrokeAlign, FVector4::Zero(), 0.0f);
	return Builder.Patch(WidgetToPatch, GetAssetOuter(), GetUniqueName());
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

void UFigmaSection::Reset()
{
	Builder.Reset();
}

TObjectPtr<UWidget> UFigmaSection::GetTopWidget() const
{
	return Builder.GetTopWidget();
}

FVector2D UFigmaSection::GetTopWidgetPosition() const
{
	return GetPosition();
}

TObjectPtr<UPanelWidget> UFigmaSection::GetContainerWidget() const
{
	return Builder.GetContainerWidget();
}

void UFigmaSection::PatchBinds(TObjectPtr<UWidgetBlueprint> WidgetBp) const
{
	if (WidgetBp == nullptr)
		return;

	TObjectPtr<UWidget> Widget = GetTopWidget();
	ProcessComponentPropertyReferences(WidgetBp, Widget);
}
