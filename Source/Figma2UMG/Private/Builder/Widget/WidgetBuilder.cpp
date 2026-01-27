// MIT License
// Copyright (c) 2024 Buvi Games


#include "Builder/Widget/WidgetBuilder.h"

#include "Figma2UMGModule.h"
#include "Blueprint/WidgetTree.h"
#include "Builder/WidgetBlueprintHelper.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/ButtonSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/GridSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/SizeBoxSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Widget.h"
#include "Components/WrapBox.h"
#include "Components/WrapBoxSlot.h"
#include "Parser/Nodes/FigmaGroup.h"
#include "Parser/Nodes/FigmaInstance.h"
#include "Parser/Nodes/FigmaNode.h"
#include "Parser/Nodes/FigmaSection.h"
#include "Parser/Nodes/Vectors/FigmaText.h"

void IWidgetBuilder::SetNode(const UFigmaNode* InNode)
{
	Node = InNode;
}

void IWidgetBuilder::SetParent(TScriptInterface<IWidgetBuilder> InParent)
{
	Parent = InParent;
}

TObjectPtr<UWidget> IWidgetBuilder::FindNodeWidgetInParent(const TObjectPtr<UPanelWidget>& ParentWidget) const
{
	if (!ParentWidget)
		return nullptr;

	TArray<UWidget*> AllChildren = ParentWidget->GetAllChildren();
	const FString IdForName = Node->GetIdForName();
	const FString NodeName = Node->GetNodeName();

	UE_LOG_Figma2UMG(Display, TEXT("[FindNodeWidgetInParent] Searching in %s for node '%s' (ID: %s), %d children available"),
		*ParentWidget->GetName(), *NodeName, *IdForName, AllChildren.Num());

	// First try to find by node ID (for widgets that include the Figma ID in their name)
	for (TObjectPtr<UWidget> Widget : AllChildren)
	{
		if (Widget == nullptr)
			continue;

		if (Widget->GetName().Contains(IdForName, ESearchCase::IgnoreCase))
		{
			UE_LOG_Figma2UMG(Display, TEXT("[FindNodeWidgetInParent] Found by ID: %s"), *Widget->GetName());
			return Widget;
		}
	}

	// Fallback: try to find by node name (for widgets named like "SizeBox-Container_1")
	for (TObjectPtr<UWidget> Widget : AllChildren)
	{
		if (Widget == nullptr)
			continue;

		if (Widget->GetName().Contains(NodeName, ESearchCase::IgnoreCase))
		{
			UE_LOG_Figma2UMG(Display, TEXT("[FindNodeWidgetInParent] Found by name: %s"), *Widget->GetName());
			return Widget;
		}
	}

	UE_LOG_Figma2UMG(Warning, TEXT("[FindNodeWidgetInParent] No widget found for node '%s' (ID: %s)"), *NodeName, *IdForName);
	return nullptr;
}

void IWidgetBuilder::PatchWidgetBinds(const TObjectPtr<UWidgetBlueprint>& WidgetBlueprint)
{
	if (WidgetBlueprint == nullptr)
	{
		UE_LOG_Figma2UMG(Error, TEXT("[PatchWidgetBinds] Missing Blueprint for node %s."), *Node->GetNodeName());
		return;
	}

	TObjectPtr<UWidget> Widget = GetWidget();
	if (Widget == nullptr)
	{
		UE_LOG_Figma2UMG(Error, TEXT("[PatchWidgetBinds] Missing Widget for node %s."), *Node->GetNodeName());
		return;
	}

	for (const TPair<FString, FString>& ComponentPropertyReference : Node->GetComponentPropertyReferences())
	{
		ProcessComponentPropertyReference(WidgetBlueprint, Widget, ComponentPropertyReference);
	}
}

TObjectPtr<UWidget> IWidgetBuilder::FindWidgetRecursive(const FString& WidgetName) const
{
	if (TObjectPtr<UWidget> Widget = GetWidget())
	{
		if (Widget->GetName().Contains(WidgetName))
		{
			return  Widget;
		}
	}

	return nullptr;
}

bool IWidgetBuilder::IsInsideComponentPackage(FString PackagePath) const
{
	if (!PackagePath.Contains("Components"))
		return false;

	TObjectPtr<UFigmaNode> TopParentNode = Node ? Node->GetParentNode() : nullptr;
	while (TopParentNode && TopParentNode->GetParentNode())
	{
		TopParentNode = TopParentNode->GetParentNode();
	}

	FString Suffix = "Components";
	FString ComponentPath = TopParentNode->GetCurrentPackagePath() + TEXT("/") + Suffix;
	return PackagePath.Contains(ComponentPath);
}

bool IWidgetBuilder::Insert(const TObjectPtr<UWidgetTree>& WidgetTree, const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget) const
{
	if (Parent)
	{
		if (Parent->TryInsertOrReplace(PrePatchWidget, PostPatchWidget))
		{
			OnInsert();
			return true;
		}
	}
	else if (WidgetTree)
	{
		WidgetTree->RootWidget = PostPatchWidget;
		OnInsert();
		return  true;
	}

	return  false;
}

void IWidgetBuilder::OnInsert() const
{
	SetPosition();
	SetRotation();
	SetSize();
	SetPadding();
	SetOpacity();

	SetConstraintsAndAlign();

	SetClipsContent();
}

bool IWidgetBuilder::IsTopWidgetForNode() const
{
	if (Parent == nullptr)
		return true;

	return (Node != Parent->Node);
}

void IWidgetBuilder::SetPosition() const
{
	const TObjectPtr<UWidget> Widget = GetWidget();
	if (Widget && Widget->Slot)
	{
		const FVector2D Position = Node->GetPosition();

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
		{
			UE_LOG_Figma2UMG(Display, TEXT("[SetPosition] Node: %s, Widget: %s, CanvasSlot Position: (%.1f, %.1f)"),
				Node ? *Node->GetNodeName() : TEXT("no node"),
				*Widget->GetName(),
				Position.X, Position.Y);
			CanvasSlot->SetPosition(Position);
		}
		// Note: HBox/VBox/WrapBox slots use auto-layout positioning, not absolute position
	}
}

void IWidgetBuilder::SetRotation() const
{
	const TObjectPtr<UWidget> Widget = GetWidget();
	if (Widget && Widget->Slot)
	{
		if (IsTopWidgetForNode())
		{
			Widget->SetRenderTransformAngle(Node->GetRotation());
		}
		else
		{
			Widget->SetRenderTransformAngle(0.0f);
		}
	}
}

bool IWidgetBuilder::IsNodeHugSizing(bool bHorizontal) const
{
	if (const UFigmaGroup* FigmaGroup = Cast<UFigmaGroup>(Node))
	{
		return bHorizontal ?
			FigmaGroup->LayoutSizingHorizontal == EFigmaLayoutSizing::HUG :
			FigmaGroup->LayoutSizingVertical == EFigmaLayoutSizing::HUG;
	}
	if (const UFigmaInstance* FigmaInstance = Cast<UFigmaInstance>(Node))
	{
		return bHorizontal ?
			FigmaInstance->LayoutSizingHorizontal == EFigmaLayoutSizing::HUG :
			FigmaInstance->LayoutSizingVertical == EFigmaLayoutSizing::HUG;
	}
	if (const UFigmaText* FigmaText = Cast<UFigmaText>(Node))
	{
		return bHorizontal ?
			FigmaText->LayoutSizingHorizontal == EFigmaLayoutSizing::HUG :
			FigmaText->LayoutSizingVertical == EFigmaLayoutSizing::HUG;
	}
	return false;
}

void IWidgetBuilder::SetSize() const
{
	FVector2D Size;
	bool SizeToContent = false;
	if(!GetSizeValue(Size, SizeToContent))
		return;

	const TObjectPtr<UWidget> Widget = GetWidget();
	if (Widget && Widget->Slot)
	{
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
		{
			CanvasSlot->SetSize(Size);

			if (SizeToContent || Widget->IsA<UUserWidget>())
			{
				CanvasSlot->SetAutoSize(true);
			}
		}
		//else if (USizeBoxSlot* SizeBoxSlot = Cast<USizeBoxSlot>(Widget->Slot))
		//{
		//	SizeBoxSlot->SetSize(Size);
		//}
		else if (UHorizontalBoxSlot* HorizontalBoxSlot = Cast<UHorizontalBoxSlot>(Widget->Slot))
		{
			FSlateChildSize ChildSize;
			ChildSize.Value = Size.X;
			ChildSize.SizeRule = SizeToContent ? ESlateSizeRule::Fill : ESlateSizeRule::Automatic;
			HorizontalBoxSlot->SetSize(ChildSize);
		}
		else if (UVerticalBoxSlot* VerticalBoxSlot = Cast<UVerticalBoxSlot>(Widget->Slot))
		{
			FSlateChildSize ChildSize;
			ChildSize.Value = Size.Y;
			// HUG = size to content = use Automatic (natural size), FILL = stretch = use Fill
			bool bUseFill = SizeToContent && !IsNodeHugSizing(false);
			ChildSize.SizeRule = bUseFill ? ESlateSizeRule::Fill : ESlateSizeRule::Automatic;
			VerticalBoxSlot->SetSize(ChildSize);
		}
		//else if (UWrapBoxSlot* WrapBoxSlot = Cast<UWrapBoxSlot>(Widget->Slot))
		//{
		//	WrapBoxSlot->SetSize(Size);
		//}
	}
}

void IWidgetBuilder::SetPadding() const
{
	FMargin Padding;
	GetPaddingValue(Padding);

	const TObjectPtr<UWidget> Widget = GetWidget();
	if (Widget && Widget->Slot)
	{
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
		{
		}
		else if (USizeBoxSlot* SizeBoxSlot = Cast<USizeBoxSlot>(Widget->Slot))
		{
			SizeBoxSlot->SetPadding(Padding);
		}
		else if (UBorderSlot* BorderSlot = Cast<UBorderSlot>(Widget->Slot))
		{
			BorderSlot->SetPadding(Padding);
		}
		else if (UHorizontalBoxSlot* HorizontalBoxSlot = Cast<UHorizontalBoxSlot>(Widget->Slot))
		{
			HorizontalBoxSlot->SetPadding(Padding);
		}
		else if (UVerticalBoxSlot* VerticalBoxSlot = Cast<UVerticalBoxSlot>(Widget->Slot))
		{
			VerticalBoxSlot->SetPadding(Padding);
		}
		else if (UWrapBoxSlot* WrapBoxSlot = Cast<UWrapBoxSlot>(Widget->Slot))
		{
			WrapBoxSlot->SetPadding(Padding);
		}
		else if (UButtonSlot* ButtonSlot = Cast<UButtonSlot>(Widget->Slot))
		{
			ButtonSlot->SetPadding(Padding);
		}
		else if (UGridSlot* GridSlot = Cast<UGridSlot>(Widget->Slot))
		{
			GridSlot->SetPadding(Padding);
		}
	}
}

void IWidgetBuilder::SetOpacity() const
{
	const TObjectPtr<UWidget> Widget = GetWidget();
	if (!Widget)
		return;

	if (IsTopWidgetForNode())
	{
		if (const UFigmaGroup* FigmaGroup = Cast<UFigmaGroup>(Node))
		{
			Widget->SetRenderOpacity(FigmaGroup->Opacity);
		}
		else if (const UFigmaInstance* FigmaInstance = Cast<UFigmaInstance>(Node))
		{
			Widget->SetRenderOpacity(FigmaInstance->Opacity);
		}
		else if (const UFigmaText* FigmaText = Cast<UFigmaText>(Node))
		{
			Widget->SetRenderOpacity(FigmaText->Opacity);
		}
		else if (const UFigmaVectorNode* FigmaVector = Cast<UFigmaVectorNode>(Node))
		{
			Widget->SetRenderOpacity(FigmaVector->Opacity);
		}
	}
	else
	{
		Widget->SetRenderOpacity(1.0f);
	}
}

void IWidgetBuilder::SetConstraintsAndAlign() const
{
	EHorizontalAlignment HorizontalAlignment = EHorizontalAlignment::HAlign_Left;
	EVerticalAlignment VerticalAlignment = EVerticalAlignment::VAlign_Top;
	if (!GetAlignmentValues(HorizontalAlignment, VerticalAlignment))
		return;

	const TObjectPtr<UWidget> Widget = GetWidget();
	if (Widget && Widget->Slot)
	{
		UE_LOG_Figma2UMG(Display, TEXT("[SetConstraintsAndAlign] Node: %s, Widget: %s (%s), Slot: %s"),
			Node ? *Node->GetNodeName() : TEXT("no node"),
			*Widget->GetName(),
			*Widget->GetClass()->GetName(),
			*Widget->Slot->GetClass()->GetName());

		if (UWrapBox* WrapBox = Cast<UWrapBox>(Widget))
		{
			WrapBox->SetHorizontalAlignment(HorizontalAlignment);
			HorizontalAlignment = HAlign_Fill;
			VerticalAlignment = VAlign_Fill;
		}

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
		{
			UE_LOG_Figma2UMG(Display, TEXT("[SetConstraintsAndAlign] Node: %s - CanvasPanelSlot (no alignment change)"),
				Node ? *Node->GetNodeName() : TEXT("no node"));
		}
		else if (USizeBoxSlot* SizeBoxSlot = Cast<USizeBoxSlot>(Widget->Slot))
		{
			UE_LOG_Figma2UMG(Display, TEXT("[SetConstraintsAndAlign] Node: %s - SizeBoxSlot -> HAlign_Fill, VAlign_Fill"),
				Node ? *Node->GetNodeName() : TEXT("no node"));
			SizeBoxSlot->SetHorizontalAlignment(HAlign_Fill);
			SizeBoxSlot->SetVerticalAlignment(VAlign_Fill);
		}
		else if (UBorderSlot* BorderSlot = Cast<UBorderSlot>(Widget->Slot))
		{
			BorderSlot->SetHorizontalAlignment(HorizontalAlignment);
			BorderSlot->SetVerticalAlignment(VerticalAlignment);
		}
		else if (UHorizontalBoxSlot* HorizontalBoxSlot = Cast<UHorizontalBoxSlot>(Widget->Slot))
		{
			UE_LOG_Figma2UMG(Display, TEXT("[SetConstraintsAndAlign] Node: %s - HorizontalBoxSlot -> HAlign: %d, VAlign: %d"),
				Node ? *Node->GetNodeName() : TEXT("no node"), (int)HorizontalAlignment, (int)VerticalAlignment);
			HorizontalBoxSlot->SetHorizontalAlignment(HorizontalAlignment);
			HorizontalBoxSlot->SetVerticalAlignment(VerticalAlignment);
		}
		else if (UVerticalBoxSlot* VerticalBoxSlot = Cast<UVerticalBoxSlot>(Widget->Slot))
		{
			UE_LOG_Figma2UMG(Display, TEXT("[SetConstraintsAndAlign] Node: %s - VerticalBoxSlot -> HAlign: %d, VAlign: %d"),
				Node ? *Node->GetNodeName() : TEXT("no node"), (int)HorizontalAlignment, (int)VerticalAlignment);
			VerticalBoxSlot->SetHorizontalAlignment(HorizontalAlignment);
			VerticalBoxSlot->SetVerticalAlignment(VerticalAlignment);
		}
		else if (UWrapBoxSlot* WrapBoxSlot = Cast<UWrapBoxSlot>(Widget->Slot))
		{
			WrapBoxSlot->SetHorizontalAlignment(HorizontalAlignment);
			WrapBoxSlot->SetVerticalAlignment(VerticalAlignment);
		}
		else if (UButtonSlot* ButtonSlot = Cast<UButtonSlot>(Widget->Slot))
		{
			ButtonSlot->SetHorizontalAlignment(HorizontalAlignment);
			ButtonSlot->SetVerticalAlignment(VerticalAlignment);
		}
		else if (UGridSlot* GridSlot = Cast<UGridSlot>(Widget->Slot))
		{
			GridSlot->SetHorizontalAlignment(HorizontalAlignment);
			GridSlot->SetVerticalAlignment(VerticalAlignment);
		}
	}
}

void IWidgetBuilder::SetClipsContent() const
{
	const TObjectPtr<UWidget> Widget = GetWidget();
	if (!Widget)
		return;

	const UFigmaGroup* FigmaGroup = Cast<UFigmaGroup>(Node);
	if(FigmaGroup)
	{
		if (FigmaGroup->ClipsContent)
		{
			Widget->SetClipping(EWidgetClipping::ClipToBounds);
		}
		else 
		{
			Widget->SetClipping(EWidgetClipping::Inherit);
		}
	}
}


void IWidgetBuilder::SetFill(const TArray<FFigmaPaint>& Fills) const
{
	const TObjectPtr<UWidget> Widget = GetWidget();
	if (!Widget)
		return;

	TObjectPtr<UBorder> Border = Cast<UBorder>(Widget);
	if (Border)
	{
		UE_LOG_Figma2UMG(Display, TEXT("[SetFill] Node: %s, Fills.Num: %d"),
			Node ? *Node->GetNodeName() : TEXT("no node"), Fills.Num());

		if (Fills.Num() > 0 && Fills[0].Visible)
		{
			if (const TObjectPtr<UMaterialInterface> Material = Fills[0].GetMaterial())
			{
				UE_LOG_Figma2UMG(Display, TEXT("[SetFill] Node: %s - Applying Material"),
					Node ? *Node->GetNodeName() : TEXT("no node"));
				Border->SetBrushColor(FLinearColor::White);
				Border->SetBrushFromMaterial(Material);
			}
			else if (const TObjectPtr<UTexture2D> Texture = Fills[0].GetTexture())
			{
				UE_LOG_Figma2UMG(Display, TEXT("[SetFill] Node: %s - Applying Texture"),
					Node ? *Node->GetNodeName() : TEXT("no node"));
				Border->SetBrushColor(FLinearColor::White);
				Border->SetBrushFromTexture(Texture);
			}
			else
			{
				const FLinearColor FillColor = Fills[0].GetLinearColor();
				UE_LOG_Figma2UMG(Display, TEXT("[SetFill] Node: %s - Applying Color: R=%.3f G=%.3f B=%.3f A=%.3f"),
					Node ? *Node->GetNodeName() : TEXT("no node"),
					FillColor.R, FillColor.G, FillColor.B, FillColor.A);

				// For solid color fills, ensure the brush has no texture/material resource
				// and is properly configured for the color
				FSlateBrush Brush = GetBrush(Border);
				Brush.SetResourceObject(nullptr);
				// Use Image draw type for solid colors - Box mode is for 9-slice images
				Brush.DrawAs = ESlateBrushDrawType::Image;
				SetBrush(Border, Brush);
				Border->SetBrushColor(FillColor);
			}
		}
		else
		{
			UE_LOG_Figma2UMG(Display, TEXT("[SetFill] Node: %s - No visible fills, setting transparent"),
				Node ? *Node->GetNodeName() : TEXT("no node"));
			Border->SetBrushColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
		}
	}
}

bool IWidgetBuilder::GetSizeValue(FVector2D& Size, bool& SizeToContent) const
{
	if(const UFigmaGroup* FigmaGroup = Cast<UFigmaGroup>(Node))
	{
		Size = FigmaGroup->GetAbsoluteSize(IsTopWidgetForNode());
		SizeToContent = FigmaGroup->LayoutSizingHorizontal == EFigmaLayoutSizing::FILL || FigmaGroup->LayoutSizingVertical == EFigmaLayoutSizing::FILL;
		return true;
	}
	
	if (const UFigmaSection* FigmaSection = Cast<UFigmaSection>(Node))
	{
		Size = FigmaSection->GetAbsoluteSize(IsTopWidgetForNode());
		SizeToContent = false;
		return true;
	}
	
	if (const UFigmaInstance* FigmaInstance = Cast<UFigmaInstance>(Node))
	{
		Size = FigmaInstance->GetAbsoluteSize(IsTopWidgetForNode());
		SizeToContent = false;
		return true;
	}
	
	if (const UFigmaText* FigmaText = Cast<UFigmaText>(Node))
	{
		Size = FigmaText->GetAbsoluteSize(IsTopWidgetForNode());
		SizeToContent = FigmaText->LayoutSizingHorizontal == EFigmaLayoutSizing::FILL || FigmaText->LayoutSizingVertical == EFigmaLayoutSizing::FILL;
		return true;
	}
	
	if (const UFigmaVectorNode* FigmaVectorNode = Cast<UFigmaVectorNode>(Node))
	{
		Size = FigmaVectorNode->GetAbsoluteSize(IsTopWidgetForNode());
		SizeToContent = false;
		return true;
	}

	return false;
}

void IWidgetBuilder::GetPaddingValue(FMargin& Padding) const
{
	Padding.Left = 0.0f;
	Padding.Right = 0.0f;
	Padding.Top = 0.0f;
	Padding.Bottom = 0.0f;

	// Only apply slot padding for the top widget of a node
	// When multiple widgets share the same node (SizeBox -> Border -> VBox),
	// the Border handles internal padding via SetPadding() in BorderWidgetBuilder::Setup()
	// Inner widgets (VBox, etc.) should NOT have additional slot padding
	if (!IsTopWidgetForNode())
	{
		return;
	}

	// Note: For top widgets, we currently don't apply the node's internal padding as slot padding
	// because the node's PaddingLeft/Right/Top/Bottom is INTERNAL padding (space inside the frame)
	// not EXTERNAL margin. The Border's content padding handles internal spacing.
	// If the top widget needs margin in its parent, that would come from a different source.
}

bool IWidgetBuilder::GetAlignmentValues(EHorizontalAlignment& HorizontalAlignment, EVerticalAlignment& VerticalAlignment) const
{
	// Check if parent is a layout container - if so, use parent's counterAxisAlignItems for cross-axis alignment
	const UFigmaGroup* ParentGroup = Node ? Cast<UFigmaGroup>(Node->GetParentNode()) : nullptr;
	const bool bParentIsVerticalLayout = ParentGroup && ParentGroup->LayoutMode == EFigmaLayoutMode::VERTICAL;
	const bool bParentIsHorizontalLayout = ParentGroup && ParentGroup->LayoutMode == EFigmaLayoutMode::HORIZONTAL;

	if (const UFigmaText* FigmaText = Cast<UFigmaText>(Node))
	{
		HorizontalAlignment = Convert(FigmaText->Style.TextAlignHorizontal);
		VerticalAlignment = Convert(FigmaText->Style.TextAlignVertical);
		//TODO: compare with FigmaText->Constraints
		return true;
	}
	else if (const UFigmaGroup* FigmaGroup = Cast<UFigmaGroup>(Node))
	{
		// For children in a layout container, determine alignment based on sizing mode
		// FILL sizing = Fill alignment, FIXED/HUG sizing = use parent's counterAxisAlignItems
		if (bParentIsVerticalLayout)
		{
			// Parent is vertical layout
			// Horizontal alignment: FILL sizing -> HAlign_Fill, otherwise use parent's counterAxisAlignItems
			if (FigmaGroup->LayoutSizingHorizontal == EFigmaLayoutSizing::FILL)
			{
				HorizontalAlignment = HAlign_Fill;
			}
			else
			{
				HorizontalAlignment = ConvertCounterAxisToHorizontal(ParentGroup->CounterAxisAlignItems);
			}
			VerticalAlignment = Convert(FigmaGroup->CounterAxisAlignItems);
		}
		else if (bParentIsHorizontalLayout)
		{
			// Parent is horizontal layout
			// Vertical alignment: FILL sizing -> VAlign_Fill, otherwise use parent's counterAxisAlignItems
			HorizontalAlignment = Convert(FigmaGroup->PrimaryAxisAlignItems);
			if (FigmaGroup->LayoutSizingVertical == EFigmaLayoutSizing::FILL)
			{
				VerticalAlignment = VAlign_Fill;
			}
			else
			{
				VerticalAlignment = ConvertCounterAxisToVertical(ParentGroup->CounterAxisAlignItems);
			}
		}
		else
		{
			// No layout parent - use the node's own settings (for absolute positioned items)
			HorizontalAlignment = Convert(FigmaGroup->PrimaryAxisAlignItems);
			VerticalAlignment = Convert(FigmaGroup->CounterAxisAlignItems);
		}
		//TODO: compare with FigmaGroup->Constraints
		return true;
	}
	else if (const UFigmaInstance* FigmaInstance = Cast<UFigmaInstance>(Node))
	{
		HorizontalAlignment = Convert(FigmaInstance->Constraints.Horizontal);
		VerticalAlignment = Convert(FigmaInstance->Constraints.Vertical);
		return true;
	}
	else if (const UFigmaVectorNode* FigmaVectorNode = Cast<UFigmaVectorNode>(Node))
	{
		HorizontalAlignment = Convert(FigmaVectorNode->Constraints.Horizontal);
		VerticalAlignment = Convert(FigmaVectorNode->Constraints.Vertical);
		return true;
	}

	return false;
}

EHorizontalAlignment IWidgetBuilder::Convert(EFigmaTextAlignHorizontal TextAlignHorizontal) const
{
	switch (TextAlignHorizontal)
	{
	case EFigmaTextAlignHorizontal::LEFT:
		return HAlign_Left;
	case EFigmaTextAlignHorizontal::CENTER:
		return HAlign_Center;
	case EFigmaTextAlignHorizontal::RIGHT:
		return HAlign_Right;
	case EFigmaTextAlignHorizontal::JUSTIFIED:
		return HAlign_Fill;
	}
	return HAlign_Center;
}

EHorizontalAlignment IWidgetBuilder::Convert(EFigmaLayoutConstraintHorizontal LayoutConstraint) const
{
	switch (LayoutConstraint)
	{
	case EFigmaLayoutConstraintHorizontal::LEFT:
		return HAlign_Left;
	case EFigmaLayoutConstraintHorizontal::RIGHT:
		return HAlign_Right;
	case EFigmaLayoutConstraintHorizontal::CENTER:
		return HAlign_Center;
	case EFigmaLayoutConstraintHorizontal::LEFT_RIGHT:
		return HAlign_Center;
	case EFigmaLayoutConstraintHorizontal::SCALE:
		return HAlign_Fill;
	}

	return HAlign_Center;
}

EHorizontalAlignment IWidgetBuilder::Convert(EFigmaPrimaryAxisAlignItems  LayoutConstraint) const
{
	switch (LayoutConstraint)
	{
	case EFigmaPrimaryAxisAlignItems::MIN:
		return HAlign_Left;
	case EFigmaPrimaryAxisAlignItems::CENTER:
		return HAlign_Center;
	case EFigmaPrimaryAxisAlignItems::MAX:
		return HAlign_Right;

	case EFigmaPrimaryAxisAlignItems::SPACE_BETWEEN:
		return HAlign_Fill;
	}

	return HAlign_Center;
}

EVerticalAlignment IWidgetBuilder::Convert(EFigmaTextAlignVertical TextAlignVertical) const
{
	switch (TextAlignVertical)
	{
	case EFigmaTextAlignVertical::TOP:
		return VAlign_Top;
	case EFigmaTextAlignVertical::CENTER:
		return VAlign_Center;
	case EFigmaTextAlignVertical::BOTTOM:
		return VAlign_Bottom;
	}

	return VAlign_Center;
}

EVerticalAlignment IWidgetBuilder::Convert(EFigmaLayoutConstraintVertical LayoutConstraint) const
{
	switch (LayoutConstraint)
	{
	case EFigmaLayoutConstraintVertical::TOP:
		return VAlign_Top;
	case EFigmaLayoutConstraintVertical::BOTTOM:
		return VAlign_Bottom;
	case EFigmaLayoutConstraintVertical::CENTER:
		return VAlign_Center;
	case EFigmaLayoutConstraintVertical::TOP_BOTTOM:
		return VAlign_Fill;
	case EFigmaLayoutConstraintVertical::SCALE:
		return VAlign_Fill;
	}

	return VAlign_Center;
}

EVerticalAlignment IWidgetBuilder::Convert(EFigmaCounterAxisAlignItems LayoutConstraint) const
{
	switch (LayoutConstraint)
	{
	case EFigmaCounterAxisAlignItems::MIN:
		return VAlign_Top;
	case EFigmaCounterAxisAlignItems::CENTER:
		return VAlign_Center;
	case EFigmaCounterAxisAlignItems::MAX:
		return VAlign_Bottom;

	case EFigmaCounterAxisAlignItems::BASELINE:
		return VAlign_Fill;
	}

	return VAlign_Center;
}

EHorizontalAlignment IWidgetBuilder::ConvertCounterAxisToHorizontal(EFigmaCounterAxisAlignItems LayoutConstraint) const
{
	switch (LayoutConstraint)
	{
	case EFigmaCounterAxisAlignItems::MIN:
		return HAlign_Left;
	case EFigmaCounterAxisAlignItems::CENTER:
		return HAlign_Center;
	case EFigmaCounterAxisAlignItems::MAX:
		return HAlign_Right;
	case EFigmaCounterAxisAlignItems::BASELINE:
		return HAlign_Fill;
	}
	return HAlign_Left;
}

EVerticalAlignment IWidgetBuilder::ConvertCounterAxisToVertical(EFigmaCounterAxisAlignItems LayoutConstraint) const
{
	switch (LayoutConstraint)
	{
	case EFigmaCounterAxisAlignItems::MIN:
		return VAlign_Top;
	case EFigmaCounterAxisAlignItems::CENTER:
		return VAlign_Center;
	case EFigmaCounterAxisAlignItems::MAX:
		return VAlign_Bottom;
	case EFigmaCounterAxisAlignItems::BASELINE:
		return VAlign_Fill;
	}
	return VAlign_Top;
}

void IWidgetBuilder::ProcessComponentPropertyReference(const TObjectPtr<UWidgetBlueprint>& WidgetBlueprint, const TObjectPtr<UWidget>& Widget, const TPair<FString, FString>& PropertyReference) const
{
	static const FString VisibleStr("visible");
	static const FString CharactersStr("characters");
	const FBPVariableDescription* VariableDescription = WidgetBlueprint->NewVariables.FindByPredicate([PropertyReference](const FBPVariableDescription& VariableDescription)
		{
#if (ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 3)
			return VariableDescription.VarName == PropertyReference.Value;
#else
			return VariableDescription.VarName.ToString() == PropertyReference.Value;
#endif
		});


	if (VariableDescription != nullptr)
	{
		UE_LOG_Figma2UMG(Display, TEXT("[ProcessComponentPropertyReference] Variable '%s' found in UWidgetBlueprint %s."), *PropertyReference.Value, *WidgetBlueprint->GetName());

		if (PropertyReference.Key == VisibleStr)
		{
			WidgetBlueprintHelper::PatchVisibilityBind(WidgetBlueprint, Widget, *PropertyReference.Value);
		}
		else if (PropertyReference.Key == CharactersStr)
		{
			TObjectPtr<UTextBlock> TextBlock = Cast<UTextBlock>(Widget);
			if (TextBlock == nullptr)
			{
				UE_LOG_Figma2UMG(Error, TEXT("[ProcessComponentPropertyReference] UWidgetBlueprint %s's Widget '%s' is not a UTextBlock. Fail to bind %s."), *WidgetBlueprint->GetName(), *Widget->GetName(), *PropertyReference.Value);
				return;
			}

			WidgetBlueprintHelper::PatchTextBind(WidgetBlueprint, TextBlock, *PropertyReference.Value);
		}
		else
		{
			UE_LOG_Figma2UMG(Warning, TEXT("[ProcessComponentPropertyReference] Unknown property '%s'."), *PropertyReference.Key);
		}

		return;
	}
	else
	{
		UClass* WidgetClass = Widget->GetClass();
		FProperty* Property = WidgetClass ? FindFProperty<FProperty>(WidgetClass, *PropertyReference.Value) : nullptr;
		if (Property)
		{
			static FString True("True");
			const FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property);
			void* Value = BoolProperty->ContainerPtrToValuePtr<uint8>(Widget);
			BoolProperty->SetPropertyValue(Value, PropertyReference.Value.Compare(True, ESearchCase::IgnoreCase) == 0);

			UE_LOG_Figma2UMG(Display, TEXT("[ProcessComponentPropertyReference] Variable '%s' found in UWidget %s."), *PropertyReference.Key, *Widget->GetName());
		}
	}

	UE_LOG_Figma2UMG(Error, TEXT("[ProcessComponentPropertyReference] Variable '%s' not found in UWidgetBlueprint %s or UWidget %s."), *PropertyReference.Value, *WidgetBlueprint->GetName(), *Widget->GetName());
}

FSlateBrush IWidgetBuilder::GetBrush(TObjectPtr<UBorder> Widget) const
{
	return Widget->Background;
}

FSlateBrush IWidgetBuilder::GetBrush(TObjectPtr<UImage> Widget) const
{
	return Widget->GetBrush();
}

void IWidgetBuilder::SetBrush(TObjectPtr<UBorder> Widget, FSlateBrush& Brush) const
{
	Widget->SetBrush(Brush);
}

void IWidgetBuilder::SetBrush(TObjectPtr<UImage> Widget, FSlateBrush& Brush) const
{
	//This is to force the update as only some of the fields are checked for change.
	Brush.ImageSize.X += 0.1f;
	Widget->SetBrush(Brush);
	Brush.ImageSize.X -= 0.1f;
	Widget->SetBrush(Brush);
}

void IWidgetBuilder::SetColorAndOpacity(TObjectPtr<UBorder> Widget, const FLinearColor& Color) const
{
	Widget->SetContentColorAndOpacity(Color);
}

void IWidgetBuilder::SetColorAndOpacity(TObjectPtr<UImage> Widget, const FLinearColor& Color) const
{
	Widget->SetColorAndOpacity(Color);
}
