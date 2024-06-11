// Copyright 2024 Buvi Games. All Rights Reserved.


#include "ButtonWidgetBuilder.h"

#include "Figma2UMGModule.h"
#include "FigmaImportSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/ContentWidget.h"


void UButtonWidgetBuilder::PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
	Widget = Cast<UButton>(WidgetToPatch);
	const FString NodeName = Node->GetNodeName();
	const FString WidgetName = Node->GetUniqueName();
	if (Widget)
	{
		UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
		UClass* ClassOverride = Importer ? Importer->GetOverrideClassForNode<UBorder>(NodeName) : nullptr;
		if (ClassOverride && Widget->GetClass() != ClassOverride)
		{
			UButton* NewButton = UFigmaImportSubsystem::NewWidget<UButton>(WidgetTree, NodeName , WidgetName, ClassOverride);
			NewButton->SetContent(Widget->GetContent());
			Widget = NewButton;
		}
		UFigmaImportSubsystem::TryRenameWidget(WidgetName, Widget);
	}
	else
	{
		Widget = UFigmaImportSubsystem::NewWidget<UButton>(WidgetTree, NodeName, WidgetName);
		if (WidgetToPatch)
		{
			Widget->SetContent(WidgetToPatch);
		}
	}

	Insert(WidgetTree, WidgetToPatch, Widget);

	Setup();

	PatchAndInsertChild(WidgetTree, Widget);
}

void UButtonWidgetBuilder::SetDefaultNode(const UFigmaGroup* InNode)
{
	DefaultNode = InNode;
	if(DefaultNode)
	{
		TScriptInterface<IWidgetBuilder> Bulder = DefaultNode->CreateWidgetBuilders(false, false);
		if(Bulder)
		{
			SetChild(Bulder);
		}
	}
}

void UButtonWidgetBuilder::SetHoveredNode(const UFigmaGroup* InNode)
{
	HoveredNode = InNode;
}

void UButtonWidgetBuilder::SetPressedNode(const UFigmaGroup* InNode)
{
	PressedNode = InNode;
}

void UButtonWidgetBuilder::SetDisabledNode(const UFigmaGroup* InNode)
{
	DisabledNode = InNode;
}

void UButtonWidgetBuilder::SetFocusedNode(const UFigmaGroup* InNode)
{
	FocusedNode = InNode;
}

void UButtonWidgetBuilder::SetWidget(const TObjectPtr<UWidget>& InWidget)
{
	Widget = Cast<UButton>(InWidget);
	SetChildWidget(Widget);
}

void UButtonWidgetBuilder::ResetWidget()
{
	Widget = nullptr;
	Super::ResetWidget();
}

TObjectPtr<UContentWidget> UButtonWidgetBuilder::GetContentWidget() const
{
	return Widget;
}

void UButtonWidgetBuilder::GetPaddingValue(FMargin& Padding) const
{
	Padding.Left = 0.0f;
	Padding.Right = 0.0f;
	Padding.Top = 0.0f;
	Padding.Bottom = 0.0f;
}

void UButtonWidgetBuilder::Setup() const
{
	FButtonStyle Style = Widget->GetStyle();

	if (DefaultNode)
	{
		SetupBrush(Style.Normal, *DefaultNode);
		const FMargin Padding(0.0f);// = DefaultNode->GetPadding();
		Style.SetNormalPadding(Padding);
	}

	if (HoveredNode)
	{
		SetupBrush(Style.Hovered, *HoveredNode);
	}

	if (PressedNode)
	{
		SetupBrush(Style.Pressed, *PressedNode);
		const FMargin Padding(0.0f);// = PressedNode->GetPadding();
		Style.SetPressedPadding(Padding);
	}

	if (DisabledNode)
	{
		SetupBrush(Style.Disabled, *DisabledNode);
	}

	if (FocusedNode)
	{
		//TODO
	}

	Widget->SetStyle(Style);
}

void UButtonWidgetBuilder::SetupBrush(FSlateBrush& Brush, const UFigmaGroup& FigmaGroup) const
{

	if (!FigmaGroup.Fills.IsEmpty() && FigmaGroup.Fills[0].Visible)
	{
		Brush.TintColor = FigmaGroup.Fills[0].GetLinearColor();
	}
	else
	{
		Brush.TintColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.0f);
	}

	if (!FigmaGroup.Strokes.IsEmpty())
	{
		Brush.OutlineSettings.Color = FigmaGroup.Strokes[0].GetLinearColor();
		Brush.OutlineSettings.Width = FigmaGroup.StrokeWeight;
	}
	else
	{
		Brush.OutlineSettings.Color = FLinearColor(1.0f, 1.0f, 1.0f, 0.0f);
		Brush.OutlineSettings.Width = 0.0f;

	}

	FVector4 Corners = FigmaGroup.RectangleCornerRadii.Num() == 4 ? FVector4(FigmaGroup.RectangleCornerRadii[0], FigmaGroup.RectangleCornerRadii[1], FigmaGroup.RectangleCornerRadii[2], FigmaGroup.RectangleCornerRadii[3])
																  : FVector4(FigmaGroup.CornerRadius, FigmaGroup.CornerRadius, FigmaGroup.CornerRadius, FigmaGroup.CornerRadius);
	Brush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
	Brush.OutlineSettings.CornerRadii = Corners;
	Brush.DrawAs = ESlateBrushDrawType::RoundedBox;
}
