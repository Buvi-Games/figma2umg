// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/FigmaSection.h"

#include "Figma2UMGModule.h"
#include "Blueprint/WidgetTree.h"
#include "Builder/Widget/BorderWidgetBuilder.h"
#include "Builder/Widget/Panels/CanvasBuilder.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void UFigmaSection::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);

	static FString DevStatusStr("devStatus");
	static FString TypeStr("type");
	if (JsonObj->HasTypedField<EJson::Object>(DevStatusStr))
	{
		const TSharedPtr<FJsonObject> DevStatusJson = JsonObj->GetObjectField(DevStatusStr);
		if (DevStatusJson->HasTypedField<EJson::String>(TypeStr))
		{
			DevStatus = JsonObj->GetStringField(TypeStr);
		}
	}

	PostSerializeProperty(JsonObj, "fills", Fills);
	PostSerializeProperty(JsonObj, "strokes", Strokes);
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

TScriptInterface<IWidgetBuilder> UFigmaSection::CreateWidgetBuilders(bool IsRoot/*= false*/, bool AllowFrameButton/*= true*/) const
{
	UCanvasBuilder* CanvasBuilder = NewObject<UCanvasBuilder>();
	CanvasBuilder->SetNode(this);

	for (const UFigmaNode* Child : Children)
	{
		if (TScriptInterface<IWidgetBuilder> SubBuilder = Child->CreateWidgetBuilders())
		{
			CanvasBuilder->AddChild(SubBuilder);
		}
	}

	bool RequireBorder = false;
	for (int i = 0; i < Fills.Num() && !RequireBorder; i++)
	{
		if (Fills[i].Visible)
			RequireBorder = true;
	}
	for (int i = 0; i < Strokes.Num() && !RequireBorder; i++)
	{
		if (Strokes[i].Visible)
			RequireBorder = true;
	}

	if (RequireBorder)
	{
		UBorderWidgetBuilder* BorderWidgetBuilder = NewObject<UBorderWidgetBuilder>();
		BorderWidgetBuilder->SetNode(this);
		BorderWidgetBuilder->SetChild(CanvasBuilder);

		return BorderWidgetBuilder;
	}

	return CanvasBuilder;
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

void UFigmaSection::SetupWidget(TObjectPtr<UWidget> Widget)
{
	if (Widget)
	{
		UE_LOG_Figma2UMG(Display, TEXT("[SetupWidget] UFigmaSection %s received a UWidget %s of type %s."), *GetNodeName(), *Widget->GetName(), *Widget->GetClass()->GetDisplayNameText().ToString());
	}

	Builder.SetupWidget(Widget);
}

void UFigmaSection::PostInsert() const
{
	TObjectPtr<UWidget> TopWidget = GetTopWidget();
	if (!TopWidget)
		return;

	IWidgetOwner::PostInsert();

	SetSize(TopWidget, AbsoluteBoundingBox.GetSize());
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
