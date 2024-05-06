// Fill out your copyright notice in the Description page of Project Settings.


#include "Builder/TextBoxBuilder.h"

#include "Figma2UMGModule.h"
#include "Components/TextBlock.h"
#include "Engine/UserInterfaceSettings.h"
#include "Parser/Properties/FigmaPaint.h"
#include "Parser/Properties/FigmaTypeStyle.h"

void FTextBoxBuilder::SetStyle(const FFigmaTypeStyle& Style)
{
	if (!TextBlock)
		return;

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

	FSlateFontInfo Font = TextBlock->GetFont();
	Font.Size = ConvertFontSizeFromDisplayToNative(Style.FontSize);
	Font.LetterSpacing = Style.LetterSpacing;
	TextBlock->SetFont(Font);
}

void FTextBoxBuilder::SetFill(const FFigmaPaint& Fill)
{
	if (!TextBlock)
		return;

	TextBlock->SetColorAndOpacity(Fill.GetLinearColor());
}

void FTextBoxBuilder::Reset()
{
	TextBlock = nullptr;
}


float FTextBoxBuilder::ConvertFontSizeFromDisplayToNative(float DisplayFontSize) const
{
	const UUserInterfaceSettings* UISettings = GetDefault<UUserInterfaceSettings>();
	const float FontDisplayDPI = UISettings->GetFontDisplayDPI();
	const float NativeSize = DisplayFontSize * FontDisplayDPI / static_cast<float>(FontConstants::RenderDPI);
	const float RoundedSize = FMath::GridSnap(NativeSize, 0.01f);
	return RoundedSize;
}

void FTextBoxBuilder::SetupWidget(TObjectPtr<UWidget> Widget)
{
	TextBlock = Cast<UTextBlock>(Widget);

	if (!TextBlock)
	{
		if (Widget)
		{
			UE_LOG_Figma2UMG(Error, TEXT("[FTextBoxBuilder::SetupWidget] Fail to setup UPanelWidget from UWidget %s of type %s."), *Widget->GetName(), *Widget->GetClass()->GetDisplayNameText().ToString());
		}
		else
		{
			UE_LOG_Figma2UMG(Warning, TEXT("[FTextBoxBuilder::SetupWidget] Fail to setup UPanelWidget from a null Widget."));
		}
	}
}
