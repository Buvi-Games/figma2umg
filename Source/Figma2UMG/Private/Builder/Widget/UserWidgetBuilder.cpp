// Copyright 2024 Buvi Games. All Rights Reserved.


#include "UserWidgetBuilder.h"

#include "Figma2UMGModule.h"
#include "FigmaImportSubsystem.h"
#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Builder/Asset/WidgetBlueprintBuilder.h"
#include "Components/Widget.h"
#include "Parser/Nodes/FigmaNode.h"
#include "Templates/WidgetTemplateBlueprintClass.h"


void UUserWidgetBuilder::SetWidgetBlueprintBuilder(const TObjectPtr<UWidgetBlueprintBuilder>& InWidgetBlueprintBuilder)
{
	WidgetBlueprintBuilder = InWidgetBlueprintBuilder;
	if (!InWidgetBlueprintBuilder)
	{
		UE_LOG_Figma2UMG(Warning, TEXT("[UUserWidgetBuilder::SetWidgetBlueprintBuilder] Node %s is receiving <null> WidgetBlueprintBuilder."), *Node->GetNodeName());
	}
}

void UUserWidgetBuilder::PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
	Widget = Cast<UUserWidget>(WidgetToPatch);

	const FString NodeName = Node->GetNodeName();
	const FString WidgetName = Node->GetUniqueName();
	if (UWidgetBlueprint* ComponentAsset = WidgetBlueprintBuilder ? WidgetBlueprintBuilder->GetAsset() : nullptr)
	{
		if (Widget && Widget.GetClass()->ClassGeneratedBy == ComponentAsset)
		{
			UFigmaImportSubsystem::TryRenameWidget(WidgetName, WidgetToPatch);
		}
		else
		{
			TSubclassOf<UUserWidget> UserWidgetClass = ComponentAsset->GetBlueprintClass();
			TSharedPtr<FWidgetTemplateBlueprintClass> Template = MakeShared<FWidgetTemplateBlueprintClass>(FAssetData(ComponentAsset), UserWidgetClass);
			Widget = Cast<UUserWidget>(Template->Create(WidgetTree));
			if (Widget)
			{
				UFigmaImportSubsystem::TryRenameWidget(WidgetName, Widget);
				Widget->CreatedFromPalette();
			}
		}
	}

	Insert(WidgetTree, WidgetToPatch, Widget);
}

bool UUserWidgetBuilder::TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget)
{
	UE_LOG_Figma2UMG(Warning, TEXT("[UUserWidgetBuilder::TryInsertOrReplace] Node %s is an UUserWidget and can't insert widgets."), *Node->GetNodeName());
	return false;
}

TObjectPtr<UWidget> UUserWidgetBuilder::GetWidget() const
{
	return Widget;
}

void UUserWidgetBuilder::GetPaddingValue(FMargin& Padding) const
{
	Padding.Left = 0.0f;
	Padding.Right = 0.0f;
	Padding.Top = 0.0f;
	Padding.Bottom = 0.0f;
}

bool UUserWidgetBuilder::GetAlignmentValues(EHorizontalAlignment& HorizontalAlignment, EVerticalAlignment& VerticalAlignment) const
{
	HorizontalAlignment = HAlign_Fill;
	VerticalAlignment = VAlign_Fill;
	return true;
};
