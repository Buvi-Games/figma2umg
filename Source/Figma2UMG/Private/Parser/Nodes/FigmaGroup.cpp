// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/FigmaGroup.h"

#include "FigmaComponentSet.h"
#include "Builder/Asset/MaterialBuilder.h"
#include "Builder/Widget/BorderWidgetBuilder.h"
#include "Builder/Widget/ButtonWidgetBuilder.h"
#include "Builder/Widget/PanelWidgetBuilder.h"
#include "Builder/Widget/SizeBoxWidgetBuilder.h"
#include "Builder/Widget/WidgetBuilder.h"
#include "Builder/Widget/Panels/CanvasBuilder.h"
#include "Builder/Widget/Panels/HBoxBuilder.h"
#include "Builder/Widget/Panels/VBoxBuilder.h"
#include "Builder/Widget/Panels/WBoxBuilder.h"
#include "Components/CanvasPanel.h"
#include "Components/Spacer.h"
#include "Components/WrapBox.h"
#include "UObject/ScriptInterface.h"

void UFigmaGroup::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);

	PostSerializeProperty(JsonObj, "fills", Fills);
	PostSerializeProperty(JsonObj, "strokes", Strokes);
}

bool UFigmaGroup::CreateAssetBuilder(const FString& InFileKey, TArray<TScriptInterface<IAssetBuilder>>& AssetBuilders)
{
	CreatePaintAssetBuilderIfNeeded(InFileKey, AssetBuilders, Fills, Strokes);

	return Super::CreateAssetBuilder(InFileKey, AssetBuilders);
}

FString UFigmaGroup::GetPackageNameForBuilder(const TScriptInterface<IAssetBuilder>& InAssetBuilder) const
{
	if (Cast<UMaterialBuilder>(InAssetBuilder.GetObject()))
	{
		TObjectPtr<UFigmaNode> TopParentNode = ParentNode;
		while (TopParentNode && TopParentNode->GetParentNode())
		{
			TopParentNode = TopParentNode->GetParentNode();
		}
		return TopParentNode->GetCurrentPackagePath() + TEXT("/") + "Material";
	}

	return Super::GetPackageNameForBuilder(InAssetBuilder);
}

TScriptInterface<IWidgetBuilder> UFigmaGroup::CreateWidgetBuilders(bool IsRoot/*= false*/, bool AllowFrameButton/*= true*/) const
{
	if (AllowFrameButton && IsButton())
	{
		TScriptInterface<UButtonWidgetBuilder> Button = CreateButtonBuilder();
		const TScriptInterface<IWidgetBuilder> Container = CreateContainersBuilder();
		Button->SetChild(Container);

#if (ENGINE_MAJOR_VERSION < 5 || ENGINE_MINOR_VERSION <= 2)
		return Button.GetInterface();
#else
		return Button;
#endif
	}
	else
	{
		TScriptInterface<IWidgetBuilder> WidgetBuilder = CreateContainersBuilder();
		return WidgetBuilder;
	}
}

FVector2D UFigmaGroup::GetAbsolutePosition() const
{
	return AbsoluteBoundingBox.GetPosition();
}

FVector2D UFigmaGroup::GetAbsoluteSize() const
{
	return AbsoluteBoundingBox.GetSize();
}

FMargin UFigmaGroup::GetPadding() const
{
	FMargin Padding;
	Padding.Left = PaddingLeft;
	Padding.Right = PaddingRight;
	Padding.Top = PaddingTop;
	Padding.Bottom = PaddingBottom;

	return Padding;
}

bool UFigmaGroup::IsButton() const
{
	if (!TransitionNodeID.IsEmpty())
	{
		return true;
	}
	else
	{
		const UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
		return  Importer ? Importer->ShouldGenerateButton(GetNodeName()) : false;
	}
}

TScriptInterface<UButtonWidgetBuilder> UFigmaGroup::CreateButtonBuilder() const
{
	UButtonWidgetBuilder* ButtonBuilder = NewObject<UButtonWidgetBuilder>();
	ButtonBuilder->SetNode(this);

	ButtonBuilder->SetDefaultNode(this);
	ButtonBuilder->SetHoveredNode(this);
	ButtonBuilder->SetPressedNode(this);
	ButtonBuilder->SetDisabledNode(this);
	ButtonBuilder->SetFocusedNode(this);

	return ButtonBuilder;
}

TScriptInterface<IWidgetBuilder> UFigmaGroup::CreateContainersBuilder() const
{
	const bool IsGeneratingButton = IsButton();
	USizeBoxWidgetBuilder* SizeBoxWidgetBuilder = nullptr;
	UBorderWidgetBuilder* BorderWidgetBuilder = nullptr;
	UPanelWidgetBuilder* PanelWidgetBuilder = nullptr;
	if (!IsGeneratingButton && (LayoutSizingHorizontal == EFigmaLayoutSizing::FIXED || LayoutSizingVertical == EFigmaLayoutSizing::FIXED))
	{
		SizeBoxWidgetBuilder = NewObject<USizeBoxWidgetBuilder>();
		SizeBoxWidgetBuilder->SetNode(this);
	}

	bool RequireBorder = false;
	if (!IsGeneratingButton && (!ParentNode || !ParentNode->IsA<UFigmaComponentSet>()))
	{
		for (int i = 0; i < Fills.Num() && !RequireBorder; i++)
		{
			if (Fills[i].Visible)
				RequireBorder = true;
		}
		for (int i = 0; i < Strokes.Num() && !RequireBorder; i++)
		{
			if (Strokes[i].Visible)
				RequireBorder = true;
		}
	}

	if (RequireBorder)
	{
		BorderWidgetBuilder = NewObject<UBorderWidgetBuilder>();
		BorderWidgetBuilder->SetNode(this);
		if (SizeBoxWidgetBuilder)
		{
			SizeBoxWidgetBuilder->SetChild(BorderWidgetBuilder);
		}
	}

	switch (LayoutMode)
	{
	case EFigmaLayoutMode::NONE:
	{
		PanelWidgetBuilder = NewObject<UCanvasBuilder>();
	}
	break;
	case EFigmaLayoutMode::HORIZONTAL:
	{
		if (LayoutWrap == EFigmaLayoutWrap::NO_WRAP)
		{
			PanelWidgetBuilder = NewObject<UHBoxBuilder>();
		}
		else
		{
			PanelWidgetBuilder = NewObject<UWBoxBuilder>();
		}
	}
	break;
	case EFigmaLayoutMode::VERTICAL:
	{
		if (LayoutWrap == EFigmaLayoutWrap::NO_WRAP)
		{
			PanelWidgetBuilder = NewObject<UVBoxBuilder>();
		}
		else
		{
			PanelWidgetBuilder = NewObject<UWBoxBuilder>();
		}
	}
	break;
	}

	PanelWidgetBuilder->SetNode(this);
	for (const UFigmaNode* Child : Children)
	{
		if (TScriptInterface<IWidgetBuilder> SubBuilder = Child->CreateWidgetBuilders())
		{
			PanelWidgetBuilder->AddChild(SubBuilder);
		}
	}


	if (BorderWidgetBuilder)
	{
		BorderWidgetBuilder->SetChild(PanelWidgetBuilder);
	}
	else if (SizeBoxWidgetBuilder)
	{
		SizeBoxWidgetBuilder->SetChild(PanelWidgetBuilder);
	}

	if (SizeBoxWidgetBuilder)
	{
		return SizeBoxWidgetBuilder;
	}

	if (BorderWidgetBuilder)
	{
		return BorderWidgetBuilder;
	}

	return PanelWidgetBuilder;
}

void UFigmaGroup::FixSpacers(const TObjectPtr<UPanelWidget>& PanelWidget) const
{
	if (!PanelWidget)
		return;

	if (PanelWidget->IsA<UCanvasPanel>() || PanelWidget->IsA<UWrapBox>())
	{
		for (int i = 0; i < PanelWidget->GetChildrenCount(); i++)
		{
			UWidget* Widget = PanelWidget->GetChildAt(i);
			if (!Widget || Widget->IsA<USpacer>())
			{
				PanelWidget->RemoveChildAt(i);
				i--;
			}
		}
		if (UWrapBox* WrapBox = Cast<UWrapBox>(PanelWidget))
		{
			WrapBox->SetInnerSlotPadding(FVector2D(ItemSpacing, CounterAxisSpacing));
		}
	}
	else
	{
		for (int i = 0; i < PanelWidget->GetChildrenCount(); i++)
		{
			UWidget* Widget = PanelWidget->GetChildAt(i);
			const bool ShouldBeSpacer = (((i + 1) % 2) == 0);
			const bool IsSpacer = Widget && Widget->IsA<USpacer>();
			if (!Widget || (IsSpacer && !ShouldBeSpacer))
			{
				PanelWidget->RemoveChildAt(i);
				i--;
			}
			else if (ShouldBeSpacer && !IsSpacer)
			{
				USpacer* Spacer = NewObject<USpacer>(PanelWidget->GetOuter());
				Spacer->SetSize(FVector2D(ItemSpacing, ItemSpacing));
				PanelWidget->InsertChildAt(i, Spacer);
			}
			else if (ShouldBeSpacer && IsSpacer)
			{
				USpacer* Spacer = Cast<USpacer>(Widget);
				Spacer->SetSize(FVector2D(ItemSpacing, ItemSpacing));
			}
		}
	}

}
