// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/Nodes/Vectors/FigmaText.h"

#include "Components/TextBlock.h"

void UFigmaText::ForEach(const FOnEachFunction& Function)
{
	if (TextBlock)
	{
		Function.ExecuteIfBound(*TextBlock);
	}
}

TObjectPtr<UWidget> UFigmaText::Patch(TObjectPtr<UWidget> WidgetToPatch)
{
	TextBlock = Cast<UTextBlock>(WidgetToPatch);
	if (TextBlock)
	{
		if (TextBlock->GetName() != GetUniqueName())
		{
			TextBlock->Rename(*GetUniqueName());
		}
	}
	else
	{
		TextBlock = NewObject<UTextBlock>(GetAssetOuter(), *GetUniqueName());
	}

	TextBlock->SetText(FText::FromString(Characters));

	switch (Style.TextAlignHorizontal)
	{
	case EFigmaTextAlignHorizontal::LEFT:
		TextBlock->SetJustification(ETextJustify::Left);
		break;
	case EFigmaTextAlignHorizontal::CENTER:
		TextBlock->SetJustification(ETextJustify::Center);
		break;
	case EFigmaTextAlignHorizontal::RIGHT:
		TextBlock->SetJustification(ETextJustify::Right);
		break;
	case EFigmaTextAlignHorizontal::JUSTIFIED:
		TextBlock->SetJustification(ETextJustify::InvariantLeft);
		break;
	}

	if (!Fills.IsEmpty())
	{
		TextBlock->SetColorAndOpacity(Fills[0].Color.GetLinearColor());
	}

	FSlateFontInfo Font = TextBlock->GetFont();
	Font.Size = Style.FontSize;
	Font.LetterSpacing = Style.LetterSpacing;
	TextBlock->SetFont(Font);

	return TextBlock;
}

TObjectPtr<UWidget> UFigmaText::GetTopWidget() const
{
	return TextBlock;
}

FVector2D UFigmaText::GetTopWidgetPosition() const
{
	return GetPosition();
}

TObjectPtr<UPanelWidget> UFigmaText::GetContainerWidget() const
{
	return nullptr;
}
