// Copyright 2024 Buvi Games. All Rights Reserved.


#include "WidgetBuilder.h"

#include "Figma2UMGModule.h"
#include "Blueprint/WidgetTree.h"
#include "Builder/WidgetBlueprintHelper.h"
#include "Components/BorderSlot.h"
#include "Components/ButtonSlot.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/PanelWidget.h"
#include "Components/SizeBoxSlot.h"
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
	for (TObjectPtr<UWidget> Widget : AllChildren)
	{
		if (Widget == nullptr)
			continue;

		if (Widget->GetName().Contains(Node->GetIdForName(), ESearchCase::IgnoreCase))
		{
			return Widget;
		}
	}

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

void IWidgetBuilder::SetOpacityValue(const float InOpacity)
{
	Opacity = InOpacity;
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
	SetSize();
	SetPadding();
	SetOpacity();

	SetConstraintsAndAlign();
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
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
		{
			CanvasSlot->SetPosition(Node->GetPosition());
			//			CanvasSlot->SetAutoSize(true);
		}
		//else if (UHorizontalBoxSlot* HorizontalBoxSlot = Cast<UHorizontalBoxSlot>(Widget->Slot))
		//{
		//	HorizontalBoxSlot->SetPosition(Position);
		//}
		//else if (UVerticalBoxSlot* VerticalBoxSlot = Cast<UVerticalBoxSlot>(Widget->Slot))
		//{
		//	VerticalBoxSlot->SetPosition(Position);
		//}
		//else if (UWrapBoxSlot* WrapBoxSlot = Cast<UWrapBoxSlot>(Widget->Slot))
		//{
		//	WrapBoxSlot->SetPosition(Position);
		//}
	}
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
			ChildSize.SizeRule = SizeToContent ? ESlateSizeRule::Fill : ESlateSizeRule::Automatic;
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
	}
}

void IWidgetBuilder::SetOpacity() const
{
	const TObjectPtr<UWidget> Widget = GetWidget();
	if (!Widget)
		return;

	Widget->SetRenderOpacity(Opacity);
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
		if (UWrapBox* WrapBox = Cast<UWrapBox>(Widget))
		{
			WrapBox->SetHorizontalAlignment(HorizontalAlignment);
			HorizontalAlignment = HAlign_Fill;
			VerticalAlignment = VAlign_Fill;
		}

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
		{
		}
		else if (USizeBoxSlot* SizeBoxSlot = Cast<USizeBoxSlot>(Widget->Slot))
		{
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
			HorizontalBoxSlot->SetHorizontalAlignment(HorizontalAlignment);
			HorizontalBoxSlot->SetVerticalAlignment(VerticalAlignment);
		}
		else if (UVerticalBoxSlot* VerticalBoxSlot = Cast<UVerticalBoxSlot>(Widget->Slot))
		{
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
	}
}

bool IWidgetBuilder::GetSizeValue(FVector2D& Size, bool& SizeToContent) const
{
	if(const UFigmaGroup* FigmaGroup = Cast<UFigmaGroup>(Node))
	{
		Size = FigmaGroup->AbsoluteBoundingBox.GetSize();
		SizeToContent = FigmaGroup->LayoutSizingHorizontal == EFigmaLayoutSizing::FILL || FigmaGroup->LayoutSizingVertical == EFigmaLayoutSizing::FILL;
		return true;
	}
	
	if (const UFigmaSection* FigmaSection = Cast<UFigmaSection>(Node))
	{
		Size = FigmaSection->AbsoluteBoundingBox.GetSize();
		SizeToContent = false;
		return true;
	}
	
	if (const UFigmaInstance* FigmaInstance = Cast<UFigmaInstance>(Node))
	{
		Size = FigmaInstance->AbsoluteBoundingBox.GetSize();
		SizeToContent = false;
		return true;
	}
	
	if (const UFigmaText* FigmaText = Cast<UFigmaText>(Node))
	{
		Size = FigmaText->AbsoluteBoundingBox.GetSize();
		SizeToContent = FigmaText->LayoutSizingHorizontal == EFigmaLayoutSizing::FILL || FigmaText->LayoutSizingVertical == EFigmaLayoutSizing::FILL;
		return true;
	}
	
	if (const UFigmaVectorNode* FigmaVectorNode = Cast<UFigmaVectorNode>(Node))
	{
		Size = FigmaVectorNode->AbsoluteBoundingBox.GetSize();
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

	if (const UFigmaGroup* FigmaGroup = Cast<UFigmaGroup>(Node))
	{
		Padding.Left = FigmaGroup->PaddingLeft;
		Padding.Right = FigmaGroup->PaddingRight;
		Padding.Top = FigmaGroup->PaddingTop;
		Padding.Bottom = FigmaGroup->PaddingBottom;
	}
	else if (const UFigmaInstance* FigmaInstance = Cast<UFigmaInstance>(Node))
	{
		Padding.Left = FigmaInstance->PaddingLeft;
		Padding.Right = FigmaInstance->PaddingRight;
		Padding.Top = FigmaInstance->PaddingTop;
		Padding.Bottom = FigmaInstance->PaddingBottom;
	}
}

bool IWidgetBuilder::GetAlignmentValues(EHorizontalAlignment& HorizontalAlignment, EVerticalAlignment& VerticalAlignment) const
{
	if (const UFigmaText* FigmaText = Cast<UFigmaText>(Node))
	{
		HorizontalAlignment = Convert(FigmaText->Style.TextAlignHorizontal);
		VerticalAlignment = Convert(FigmaText->Style.TextAlignVertical);
		//TODO: compare with FigmaText->Constraints
		return true;
	}
	else if (const UFigmaGroup* FigmaGroup = Cast<UFigmaGroup>(Node))
	{
		HorizontalAlignment = Convert(FigmaGroup->PrimaryAxisAlignItems);
		VerticalAlignment = Convert(FigmaGroup->CounterAxisAlignItems);
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
