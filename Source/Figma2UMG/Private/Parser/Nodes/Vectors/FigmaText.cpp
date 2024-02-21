// Fill out your copyright notice in the Description page of Project Settings.


#include "Parser/Nodes/Vectors/FigmaText.h"

#include "WidgetBlueprint.h"
#include "Builder/WidgetBlueprintBuilder.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"

FVector2D UFigmaText::GetAbsolutePosition() const
{
	return AbsoluteBoundingBox.GetPosition();
}

FVector2D UFigmaText::GetSize() const
{
	return AbsoluteBoundingBox.GetSize();
}

void UFigmaText::ForEach(const FOnEachFunction& Function)
{
	if (Builder.TextBlock)
	{
		Function.ExecuteIfBound(*Builder.TextBlock);
	}
}

TObjectPtr<UWidget> UFigmaText::Patch(TObjectPtr<UWidget> WidgetToPatch)
{
	Builder.TextBlock = Cast<UTextBlock>(WidgetToPatch);
	if (Builder.TextBlock)
	{
		if (Builder.TextBlock->GetName() != GetUniqueName())
		{
			Builder.TextBlock->Rename(*GetUniqueName());
		}
	}
	else
	{
		Builder.TextBlock = NewObject<UTextBlock>(GetAssetOuter(), *GetUniqueName());
	}

	Builder.TextBlock->SetText(FText::FromString(Characters));

	Builder.SetStyle(Style);
	
	if (!Fills.IsEmpty())
	{
		Builder.SetFill(Fills[0]);
	}

	return Builder.TextBlock;
}

void UFigmaText::PostInsert() const
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

TObjectPtr<UWidget> UFigmaText::GetTopWidget() const
{
	return Builder.TextBlock;
}

FVector2D UFigmaText::GetTopWidgetPosition() const
{
	return GetPosition();
}

TObjectPtr<UPanelWidget> UFigmaText::GetContainerWidget() const
{
	return nullptr;
}

void UFigmaText::PatchBinds(TObjectPtr<UWidgetBlueprint> WidgetBp) const
{
	if (WidgetBp == nullptr)
		return;

	ProcessComponentPropertyReferences(WidgetBp, Builder.TextBlock);
}

void UFigmaText::ProcessComponentPropertyReference(TObjectPtr<UWidgetBlueprint> WidgetBP, TObjectPtr<UWidget> Widget, const TPair<FString, FString>& PropertyReference) const
{
	static const FString CharactersStr("characters");
	if (PropertyReference.Key == CharactersStr)
	{
		const FBPVariableDescription* VariableDescription = WidgetBP->NewVariables.FindByPredicate([PropertyReference](const FBPVariableDescription& VariableDescription)
			{
				return VariableDescription.VarName == PropertyReference.Value;
			});

		if (VariableDescription == nullptr)
			return;

		TObjectPtr<UTextBlock> TextBlock = Cast<UTextBlock>(Widget);
		if (TextBlock == nullptr)
			return;

		WidgetBlueprintBuilder::PatchTextBind(WidgetBP, TextBlock, *PropertyReference.Value);
	}
	else
	{
		Super::ProcessComponentPropertyReference(WidgetBP, Widget, PropertyReference);
	}
}

void UFigmaText::Reset()
{
	Builder.Reset();
}
