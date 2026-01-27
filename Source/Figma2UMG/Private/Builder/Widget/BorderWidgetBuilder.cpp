// MIT License
// Copyright (c) 2024 Buvi Games


#include "Builder/Widget/BorderWidgetBuilder.h"

#include "FigmaImportSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Parser/Nodes/FigmaGroup.h"
#include "Parser/Nodes/FigmaNode.h"
#include "Parser/Nodes/FigmaSection.h"


void UBorderWidgetBuilder::PatchAndInsertWidget(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UWidget>& WidgetToPatch, TMap<FString, int32>& NameTracker)
{
	if (const USizeBox* SizeBoxWrapper = Cast<USizeBox>(WidgetToPatch))
	{
		Widget = Cast<UBorder>(SizeBoxWrapper->GetContent());
	}
	else
	{
		Widget = Cast<UBorder>(WidgetToPatch);
	}

	const FString NodeName = Node->GetNodeName();
	const FString WidgetName = "Border-" + Node->GetUniqueName();
	if (Widget)
	{
		UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
		UClass* ClassOverride = Importer ? Importer->GetOverrideClassForNode<UBorder>(NodeName) : nullptr;
		if (ClassOverride && Widget->GetClass() != ClassOverride)
		{
			UBorder* NewBorder = UFigmaImportSubsystem::NewWidget<UBorder>(WidgetBlueprint->WidgetTree, NodeName, WidgetName, ClassOverride, NameTracker);
			NewBorder->SetContent(Widget->GetContent());
			Widget = NewBorder;
		}
		UFigmaImportSubsystem::TryRenameWidget(WidgetName, Widget);
	}
	else
	{
		Widget = UFigmaImportSubsystem::NewWidget<UBorder>(WidgetBlueprint->WidgetTree, NodeName, WidgetName, NameTracker);

		if (WidgetToPatch)
		{
			Widget->SetContent(WidgetToPatch);
		}
	}

	Insert(WidgetBlueprint->WidgetTree, WidgetToPatch, Widget);

	Setup();

	PatchAndInsertChild(WidgetBlueprint, Widget, NameTracker);
}

void UBorderWidgetBuilder::SetWidget(const TObjectPtr<UWidget>& InWidget)
{
	UE_LOG_Figma2UMG(Display, TEXT("[UBorderWidgetBuilder::SetWidget] Node: %s, InWidget: %s (Type: %s)"),
		Node ? *Node->GetNodeName() : TEXT("no node"),
		InWidget ? *InWidget->GetName() : TEXT("NULL"),
		InWidget ? *InWidget->GetClass()->GetName() : TEXT("N/A"));
	Widget = Cast<UBorder>(InWidget);
	if (!Widget && InWidget)
	{
		UE_LOG_Figma2UMG(Warning, TEXT("[UBorderWidgetBuilder::SetWidget] Cast to UBorder failed! InWidget type: %s"), *InWidget->GetClass()->GetName());
	}
	SetChildWidget(Widget);

	// Call Setup to apply fills, strokes, and padding to the existing widget
	// This is needed because SetWidget is called when matching builders to existing widgets
	// and those widgets may not have had Setup called on them
	if (Widget)
	{
		Setup();
	}
}

void UBorderWidgetBuilder::ResetWidget()
{
	Super::ResetWidget();
	Widget = nullptr;
}

TObjectPtr<UContentWidget> UBorderWidgetBuilder::GetContentWidget() const
{
	return Widget;
}

void UBorderWidgetBuilder::GetPaddingValue(FMargin& Padding) const
{
	Padding.Left = 0.0f;
	Padding.Right = 0.0f;
	Padding.Top = 0.0f;
	Padding.Bottom = 0.0f;
}

bool UBorderWidgetBuilder::GetAlignmentValues(EHorizontalAlignment& HorizontalAlignment, EVerticalAlignment& VerticalAlignment) const
{
	HorizontalAlignment = HAlign_Fill;
	VerticalAlignment = VAlign_Fill;
	return true;
}

void UBorderWidgetBuilder::Setup() const
{
	UE_LOG_Figma2UMG(Display, TEXT("[BorderWidgetBuilder::Setup] Node: %s, Widget: %s"),
		Node ? *Node->GetNodeName() : TEXT("no node"),
		Widget ? *Widget->GetName() : TEXT("NULL"));

	FSlateBrush Brush = Widget->Background;
	if (Node->IsA<UFigmaSection>())
	{
		Brush.DrawAs = ESlateBrushDrawType::Image;
		UE_LOG_Figma2UMG(Display, TEXT("[BorderWidgetBuilder::Setup] Node: %s - DrawAs: Image"),
			Node ? *Node->GetNodeName() : TEXT("no node"));
	}
	else
	{
		// Use Box instead of RoundedBox for solid color fills to ensure proper rendering
		Brush.DrawAs = ESlateBrushDrawType::RoundedBox;
		UE_LOG_Figma2UMG(Display, TEXT("[BorderWidgetBuilder::Setup] Node: %s - DrawAs: Box"),
			Node ? *Node->GetNodeName() : TEXT("no node"));
	}
	SetBrush(Widget, Brush);

	if (const UFigmaSection* FigmaSection = Cast<UFigmaSection>(Node))
	{
		UE_LOG_Figma2UMG(Display, TEXT("[BorderWidgetBuilder::Setup] Node: %s - FigmaSection with %d fills"),
			Node ? *Node->GetNodeName() : TEXT("no node"), FigmaSection->Fills.Num());
		SetFill(FigmaSection->Fills);
		SetStroke(Widget, FigmaSection->Strokes, FigmaSection->StrokeWeight);
	}
	else if(const UFigmaGroup* FigmaGroup = Cast<UFigmaGroup>(Node))
	{
		UE_LOG_Figma2UMG(Display, TEXT("[BorderWidgetBuilder::Setup] Node: %s - FigmaGroup with %d fills"),
			Node ? *Node->GetNodeName() : TEXT("no node"), FigmaGroup->Fills.Num());
		SetFill(FigmaGroup->Fills);
		SetStroke(Widget, FigmaGroup->Strokes, FigmaGroup->StrokeWeight);

		const FVector4 Corners = FigmaGroup->RectangleCornerRadii.Num() == 4 ? FVector4(FigmaGroup->RectangleCornerRadii[0], FigmaGroup->RectangleCornerRadii[1], FigmaGroup->RectangleCornerRadii[2], FigmaGroup->RectangleCornerRadii[3])
																			 : FVector4(FigmaGroup->CornerRadius, FigmaGroup->CornerRadius, FigmaGroup->CornerRadius, FigmaGroup->CornerRadius);
		SetCorner(Widget, Corners);

		// Apply frame's internal padding as Border content padding
		// In Figma, padding is space INSIDE the frame around children
		// In UMG Border, SetPadding() sets the content padding (space between border edge and child)
		// FMargin ContentPadding(FigmaGroup->PaddingLeft, FigmaGroup->PaddingTop, FigmaGroup->PaddingRight, FigmaGroup->PaddingBottom);
		// Widget->SetPadding(ContentPadding);
		// UE_LOG_Figma2UMG(Display, TEXT("[BorderWidgetBuilder::Setup] Node: %s - Content Padding: L=%.1f T=%.1f R=%.1f B=%.1f"),
		// 	Node ? *Node->GetNodeName() : TEXT("no node"),
		// 	FigmaGroup->PaddingLeft, FigmaGroup->PaddingTop, FigmaGroup->PaddingRight, FigmaGroup->PaddingBottom);
	}
}
