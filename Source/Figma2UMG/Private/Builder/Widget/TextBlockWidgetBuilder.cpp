// Copyright 2024 Buvi Games. All Rights Reserved.


#include "TextBlockWidgetBuilder.h"

#include "Figma2UMGModule.h"
#include "FigmaImportSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "Engine/UserInterfaceSettings.h"
#include "Parser/Nodes/FigmaNode.h"
#include "Parser/Nodes/Vectors/FigmaText.h"

void UTextBlockWidgetBuilder::PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
	Widget = Cast<UTextBlock>(WidgetToPatch);

	const FString NodeName = Node->GetNodeName();
	const FString WidgetName = Node->GetUniqueName();
	if (Widget)
	{
		UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
		UClass* ClassOverride = Importer ? Importer->GetOverrideClassForNode<UTextBlock>(NodeName) : nullptr;
		if (ClassOverride && Widget->GetClass() != ClassOverride)
		{
			Widget = UFigmaImportSubsystem::NewWidget<UTextBlock>(WidgetTree, NodeName, WidgetName, ClassOverride);
		}

		UFigmaImportSubsystem::TryRenameWidget(WidgetName, Widget);
	}
	else
	{
		Widget = UFigmaImportSubsystem::NewWidget<UTextBlock>(WidgetTree, NodeName, WidgetName);
	}

	Insert(WidgetTree, WidgetToPatch, Widget);

	Setup();
}

bool UTextBlockWidgetBuilder::TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget)
{
	UE_LOG_Figma2UMG(Warning, TEXT("[UTextBlockWidgetBuilder::TryInsertOrReplace] Node %s is an TextBlock and can't insert widgets."), *Node->GetNodeName());
	return false;
}

TObjectPtr<UWidget> UTextBlockWidgetBuilder::GetWidget() const
{
	return Widget;
}

void UTextBlockWidgetBuilder::Setup() const
{
	const UFigmaText* FigmaText = Cast<UFigmaText>(Node);
	if(!FigmaText)
	{
		UE_LOG_Figma2UMG(Warning, TEXT("[UTextBlockWidgetBuilder::Setup] Expecting Node %s to be an UFigmaText nit it's %s instead."), *Node->GetNodeName(), *Node->GetClass()->GetName());
	}

	Widget->SetText(FText::FromString(FigmaText->Characters));
	Widget->SetAutoWrapText(true);

	SetStyle(FigmaText->Style);

	if (!FigmaText->Fills.IsEmpty())
	{
		Widget->SetColorAndOpacity(FigmaText->Fills[0].GetLinearColor());
		if (FigmaText->Fills.Num() > 1)
		{
			UE_LOG_Figma2UMG(Warning, TEXT("[UTextBlockWidgetBuilder::Setup] Node %s has &i FFigmaPaints, only 1 is supported. Using the first."), *Node->GetNodeName(), FigmaText->Fills.Num());
		}
	}
}

void UTextBlockWidgetBuilder::SetStyle(const FFigmaTypeStyle& Style) const
{
	if (!Widget)
		return;

	switch (Style.TextAlignHorizontal)
	{
	case EFigmaTextAlignHorizontal::LEFT:
		Widget->SetJustification(ETextJustify::Left);
		break;
	case EFigmaTextAlignHorizontal::CENTER:
		Widget->SetJustification(ETextJustify::Center);
		break;
	case EFigmaTextAlignHorizontal::RIGHT:
		Widget->SetJustification(ETextJustify::Right);
		break;
#if (ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 3)
	case EFigmaTextAlignHorizontal::JUSTIFIED:
		Widget->SetJustification(ETextJustify::InvariantLeft);
		break;
#endif
	}

	FSlateFontInfo Font = Widget->GetFont();
	Font.Size = ConvertFontSizeFromDisplayToNative(Style.FontSize);
	Font.LetterSpacing = Style.LetterSpacing;
	Widget->SetFont(Font);
}

float UTextBlockWidgetBuilder::ConvertFontSizeFromDisplayToNative(float DisplayFontSize) const
{
#if (ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 3)
	const UUserInterfaceSettings* UISettings = GetDefault<UUserInterfaceSettings>();
	const float FontDisplayDPI = UISettings->GetFontDisplayDPI();
	const float NativeSize = DisplayFontSize * FontDisplayDPI / static_cast<float>(FontConstants::RenderDPI);
	const float RoundedSize = FMath::GridSnap(NativeSize, 0.01f);
	return RoundedSize;
#else
	return DisplayFontSize;
#endif
}