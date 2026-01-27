// MIT License
// Copyright (c) 2024 Buvi Games


#include "Parser/Nodes/FigmaGroup.h"

#include "Figma2UMGModule.h"
#include "Parser/Nodes/FigmaComponentSet.h"
#include "Builder/Asset/MaterialBuilder.h"
#include "Builder/Widget/BorderWidgetBuilder.h"
#include "Builder/Widget/ButtonWidgetBuilder.h"
#include "Builder/Widget/PanelWidgetBuilder.h"
#include "Builder/Widget/SizeBoxWidgetBuilder.h"
#include "Builder/Widget/WidgetBuilder.h"
#include "Builder/Widget/Panels/CanvasBuilder.h"
#include "Builder/Widget/Panels/GridBuilder.h"
#include "Builder/Widget/Panels/HBoxBuilder.h"
#include "Builder/Widget/Panels/VBoxBuilder.h"
#include "Builder/Widget/Panels/WBoxBuilder.h"
#include "Components/CanvasPanel.h"
#include "Components/Spacer.h"
#include "Components/WrapBox.h"
#include "Parser/Properties/FigmaAction.h"
#include "Parser/Properties/FigmaTrigger.h"
#include "UObject/ScriptInterface.h"

void UFigmaGroup::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);

	PostSerializeProperty(JsonObj, "fills", Fills);
	PostSerializeProperty(JsonObj, "strokes", Strokes);
	PostSerializeProperty(JsonObj, "interactions", Interactions);
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
		const TScriptInterface<IWidgetBuilder> Container = CreateContainersBuilder(IsRoot);
		Button->SetChild(Container);

#if (ENGINE_MAJOR_VERSION < 5 || ENGINE_MINOR_VERSION <= 2)
		return Button.GetInterface();
#else
		return Button;
#endif
	}
	else
	{
		TScriptInterface<IWidgetBuilder> WidgetBuilder = CreateContainersBuilder(IsRoot);
		return WidgetBuilder;
	}
}

FVector2D UFigmaGroup::GetAbsolutePosition(const bool IsTopWidgetForNode) const
{
	const float CurrentRotation = IsTopWidgetForNode ? GetAbsoluteRotation() : 0.0f;
	return AbsoluteBoundingBox.GetPosition(CurrentRotation);
}

FVector2D UFigmaGroup::GetAbsoluteSize(const bool IsTopWidgetForNode) const
{
	return AbsoluteBoundingBox.GetSize(IsTopWidgetForNode ? GetAbsoluteRotation() : 0.0f);
}

FVector2D UFigmaGroup::GetAbsoluteCenter() const
{
	return AbsoluteBoundingBox.GetCenter();
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

const FFigmaInteraction& UFigmaGroup::GetInteractionFromTrigger(const EFigmaTriggerType TriggerType) const
{
	return UFigmaNode::GetInteractionFromTrigger(Interactions, TriggerType);
}

const FFigmaInteraction& UFigmaGroup::GetInteractionFromAction(const EFigmaActionType ActionType, const EFigmaActionNodeNavigation Navigation) const
{
	return UFigmaNode::GetInteractionFromAction(Interactions, ActionType, Navigation);
}

const FString& UFigmaGroup::GetDestinationIdFromEvent(const FName& EventName) const
{
	const FFigmaInteraction& Interaction = GetInteractionFromAction(EFigmaActionType::NODE, EFigmaActionNodeNavigation::NAVIGATE);
	if (!Interaction.Trigger || !Interaction.Trigger->MatchEvent(EventName.ToString()))
		return TransitionNodeID;

	const UFigmaNodeAction* Action = Interaction.FindActionNode(EFigmaActionNodeNavigation::NAVIGATE);
	if (!Action || Action->DestinationId.IsEmpty())
		return TransitionNodeID;

	return Action->DestinationId;
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

TScriptInterface<IWidgetBuilder> UFigmaGroup::CreateContainersBuilder(bool IsRoot/*= false*/) const
{
	const bool IsGeneratingButton = IsButton();
	USizeBoxWidgetBuilder* SizeBoxWidgetBuilder = nullptr;
	UBorderWidgetBuilder* BorderWidgetBuilder = nullptr;
	UPanelWidgetBuilder* PanelWidgetBuilder = nullptr;

	// Skip SizeBox for root nodes - they should fill their container (e.g., HUD filling viewport)
	UE_LOG_Figma2UMG(Display, TEXT("[CreateContainersBuilder] Node: %s, IsRoot: %d, LayoutSizingH: %d, LayoutSizingV: %d"),
		*GetNodeName(),
		IsRoot ? 1 : 0,
		(int)LayoutSizingHorizontal,
		(int)LayoutSizingVertical);
	if (!IsGeneratingButton && !IsRoot && (LayoutSizingHorizontal == EFigmaLayoutSizing::FIXED || LayoutSizingVertical == EFigmaLayoutSizing::FIXED))
	{
		UE_LOG_Figma2UMG(Display, TEXT("[CreateContainersBuilder] Creating SizeBox for node: %s"), *GetNodeName());
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

	UE_LOG_Figma2UMG(Display, TEXT("[CreateContainersBuilder] Node: %s, LayoutMode: %d (NONE=0, HORIZONTAL=1, VERTICAL=2, GRID=3), LayoutSizingH: %d, LayoutSizingV: %d, LayoutWrap: %d"),
		*GetNodeName(), (int)LayoutMode, (int)LayoutSizingHorizontal, (int)LayoutSizingVertical, (int)LayoutWrap);

	switch (LayoutMode)
	{
	case EFigmaLayoutMode::NONE:
	{
		UE_LOG_Figma2UMG(Display, TEXT("[CreateContainersBuilder] Node: %s -> Creating CanvasBuilder"), *GetNodeName());
		PanelWidgetBuilder = NewObject<UCanvasBuilder>();
	}
	break;
	case EFigmaLayoutMode::HORIZONTAL:
	{
		if (LayoutWrap == EFigmaLayoutWrap::NO_WRAP)
		{
			UE_LOG_Figma2UMG(Display, TEXT("[CreateContainersBuilder] Node: %s -> Creating HBoxBuilder"), *GetNodeName());
			PanelWidgetBuilder = NewObject<UHBoxBuilder>();
		}
		else
		{
			UE_LOG_Figma2UMG(Display, TEXT("[CreateContainersBuilder] Node: %s -> Creating WBoxBuilder (horizontal wrap)"), *GetNodeName());
			PanelWidgetBuilder = NewObject<UWBoxBuilder>();
		}
	}
	break;
	case EFigmaLayoutMode::VERTICAL:
	{
		if (LayoutWrap == EFigmaLayoutWrap::NO_WRAP)
		{
			UE_LOG_Figma2UMG(Display, TEXT("[CreateContainersBuilder] Node: %s -> Creating VBoxBuilder"), *GetNodeName());
			PanelWidgetBuilder = NewObject<UVBoxBuilder>();
		}
		else
		{
			UE_LOG_Figma2UMG(Display, TEXT("[CreateContainersBuilder] Node: %s -> Creating WBoxBuilder (vertical wrap)"), *GetNodeName());
			PanelWidgetBuilder = NewObject<UWBoxBuilder>();
		}
	}
	break;
	case EFigmaLayoutMode::GRID:
	{
		UE_LOG_Figma2UMG(Display, TEXT("[CreateContainersBuilder] Node: %s -> Creating GridBuilder"), *GetNodeName());
		PanelWidgetBuilder = NewObject<UGridBuilder>();
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

TArray<FFigmaGridTrackSizing> UFigmaGroup::GetParsedColumnSizing() const
{
	if (!GridColumnsSizing.IsEmpty())
	{
		return ParseGridSizingString(GridColumnsSizing);
	}

	// Default: create equal fractional columns based on GridColumnCount
	TArray<FFigmaGridTrackSizing> Result;
	const int32 Count = FMath::Max(1, GridColumnCount);
	for (int32 i = 0; i < Count; i++)
	{
		Result.Add(FFigmaGridTrackSizing(EFigmaGridTrackType::Fractional, 1.0f));
	}
	return Result;
}

TArray<FFigmaGridTrackSizing> UFigmaGroup::GetParsedRowSizing() const
{
	if (!GridRowsSizing.IsEmpty())
	{
		return ParseGridSizingString(GridRowsSizing);
	}

	// Default: create equal fractional rows based on GridRowCount
	TArray<FFigmaGridTrackSizing> Result;
	const int32 Count = FMath::Max(1, GridRowCount);
	for (int32 i = 0; i < Count; i++)
	{
		Result.Add(FFigmaGridTrackSizing(EFigmaGridTrackType::Fractional, 1.0f));
	}
	return Result;
}

FFigmaGridTrackSizing UFigmaGroup::ParseSingleTrackValue(const FString& Value)
{
	FFigmaGridTrackSizing Track;
	FString TrimmedValue = Value.TrimStartAndEnd();

	// Check for fractional units (e.g., "1fr", "2.5fr", "400fr")
	if (TrimmedValue.EndsWith(TEXT("fr"), ESearchCase::IgnoreCase))
	{
		Track.Type = EFigmaGridTrackType::Fractional;
		FString ValueStr = TrimmedValue.LeftChop(2); // Remove "fr"
		Track.Value = FCString::Atof(*ValueStr);
		if (Track.Value <= 0.0f)
		{
			Track.Value = 1.0f;
		}
	}
	// Check for pixel units (e.g., "100px", "200.5px")
	else if (TrimmedValue.EndsWith(TEXT("px"), ESearchCase::IgnoreCase))
	{
		Track.Type = EFigmaGridTrackType::Fixed;
		FString ValueStr = TrimmedValue.LeftChop(2); // Remove "px"
		Track.Value = FCString::Atof(*ValueStr);
	}
	// Check for "auto" keyword
	else if (TrimmedValue.Equals(TEXT("auto"), ESearchCase::IgnoreCase))
	{
		Track.Type = EFigmaGridTrackType::Auto;
		Track.Value = 0.0f;
	}
	// Assume numeric values without units are pixels
	else if (TrimmedValue.IsNumeric())
	{
		Track.Type = EFigmaGridTrackType::Fixed;
		Track.Value = FCString::Atof(*TrimmedValue);
	}
	else
	{
		// Unknown format, default to auto
		Track.Type = EFigmaGridTrackType::Auto;
		Track.Value = 0.0f;
	}

	return Track;
}

TArray<FFigmaGridTrackSizing> UFigmaGroup::ParseGridSizingString(const FString& SizingString)
{
	TArray<FFigmaGridTrackSizing> Result;

	// Handle CSS Grid functions like minmax() and repeat()
	// Examples:
	// - "minmax(0, 400fr) minmax(0, 1fr)" -> space-separated minmax expressions
	// - "repeat(1, minmax(0, 1fr))" -> repeat function
	// - "1fr, 2fr, auto" -> simple comma-separated values

	FString WorkingString = SizingString.TrimStartAndEnd();

	// Check for repeat() function: repeat(N, value)
	if (WorkingString.StartsWith(TEXT("repeat("), ESearchCase::IgnoreCase))
	{
		// Extract content inside repeat()
		int32 OpenParen = WorkingString.Find(TEXT("("));
		int32 CloseParen = WorkingString.Find(TEXT(")"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		if (OpenParen != INDEX_NONE && CloseParen != INDEX_NONE)
		{
			FString RepeatContent = WorkingString.Mid(OpenParen + 1, CloseParen - OpenParen - 1);
			// Find the first comma to separate count from value
			int32 FirstComma = RepeatContent.Find(TEXT(","));
			if (FirstComma != INDEX_NONE)
			{
				FString CountStr = RepeatContent.Left(FirstComma).TrimStartAndEnd();
				FString ValuePart = RepeatContent.Mid(FirstComma + 1).TrimStartAndEnd();
				int32 RepeatCount = FCString::Atoi(*CountStr);
				if (RepeatCount <= 0) RepeatCount = 1;

				// Parse the value part (could be minmax or simple value)
				FFigmaGridTrackSizing Track;
				if (ValuePart.StartsWith(TEXT("minmax("), ESearchCase::IgnoreCase))
				{
					// Extract the max value from minmax(min, max)
					int32 MinmaxOpen = ValuePart.Find(TEXT("("));
					int32 MinmaxClose = ValuePart.Find(TEXT(")"));
					if (MinmaxOpen != INDEX_NONE && MinmaxClose != INDEX_NONE)
					{
						FString MinmaxContent = ValuePart.Mid(MinmaxOpen + 1, MinmaxClose - MinmaxOpen - 1);
						int32 MinmaxComma = MinmaxContent.Find(TEXT(","));
						if (MinmaxComma != INDEX_NONE)
						{
							FString MaxValue = MinmaxContent.Mid(MinmaxComma + 1).TrimStartAndEnd();
							Track = ParseSingleTrackValue(MaxValue);
						}
					}
				}
				else
				{
					Track = ParseSingleTrackValue(ValuePart);
				}

				// Add the track RepeatCount times
				for (int32 i = 0; i < RepeatCount; i++)
				{
					Result.Add(Track);
				}
			}
		}

		if (Result.Num() > 0)
		{
			return Result;
		}
	}

	// Check for minmax() functions (space-separated)
	// Example: "minmax(0, 400fr) minmax(0, 1fr)"
	if (WorkingString.Contains(TEXT("minmax("), ESearchCase::IgnoreCase))
	{
		// Find all minmax() expressions
		int32 SearchStart = 0;
		while (SearchStart < WorkingString.Len())
		{
			int32 MinmaxStart = WorkingString.Find(TEXT("minmax("), ESearchCase::IgnoreCase, ESearchDir::FromStart, SearchStart);
			if (MinmaxStart == INDEX_NONE)
			{
				break;
			}

			// Find the matching closing parenthesis
			int32 ParenDepth = 0;
			int32 MinmaxEnd = INDEX_NONE;
			for (int32 i = MinmaxStart; i < WorkingString.Len(); i++)
			{
				if (WorkingString[i] == '(')
				{
					ParenDepth++;
				}
				else if (WorkingString[i] == ')')
				{
					ParenDepth--;
					if (ParenDepth == 0)
					{
						MinmaxEnd = i;
						break;
					}
				}
			}

			if (MinmaxEnd != INDEX_NONE)
			{
				// Extract minmax content
				int32 ContentStart = MinmaxStart + 7; // Length of "minmax("
				FString MinmaxContent = WorkingString.Mid(ContentStart, MinmaxEnd - ContentStart);

				// Find comma separating min and max
				int32 CommaPos = MinmaxContent.Find(TEXT(","));
				if (CommaPos != INDEX_NONE)
				{
					FString MaxValue = MinmaxContent.Mid(CommaPos + 1).TrimStartAndEnd();
					Result.Add(ParseSingleTrackValue(MaxValue));
				}

				SearchStart = MinmaxEnd + 1;
			}
			else
			{
				break;
			}
		}

		if (Result.Num() > 0)
		{
			return Result;
		}
	}

	// Fall back to simple parsing: comma or space separated values like "1fr, 2fr, auto"
	TArray<FString> Parts;
	SizingString.ParseIntoArray(Parts, TEXT(","), true);

	// If no commas, try spaces (but be careful with minmax which was already handled above)
	if (Parts.Num() <= 1 && !SizingString.Contains(TEXT("(")))
	{
		Parts.Empty();
		SizingString.ParseIntoArray(Parts, TEXT(" "), true);
	}

	for (const FString& Part : Parts)
	{
		FString TrimmedPart = Part.TrimStartAndEnd();
		if (TrimmedPart.IsEmpty())
		{
			continue;
		}

		Result.Add(ParseSingleTrackValue(TrimmedPart));
	}

	// If nothing was parsed, return a single auto track
	if (Result.Num() == 0)
	{
		Result.Add(FFigmaGridTrackSizing(EFigmaGridTrackType::Auto, 0.0f));
	}

	return Result;
}
