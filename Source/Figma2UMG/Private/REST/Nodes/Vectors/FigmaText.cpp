// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/Nodes/Vectors/FigmaText.h"

#include "Components/TextBlock.h"

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
