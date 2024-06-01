#pragma once


#include "CoreMinimal.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "Components/WrapBox.h"

#include "ClassOverrides.generated.h"

UENUM()
enum class EOverrideConditionCheck
{
	StartsWith,
	Contains,
	WildCard,
};

USTRUCT()
struct FIGMA2UMG_API FWidgetOverride
{
	GENERATED_BODY()
public:
	FWidgetOverride() = default;

	bool Match(const FString& NodeName) const;

	UPROPERTY(EditAnywhere, Category = "Figma2UMG")
	bool HasCondition = false;

	UPROPERTY(EditAnywhere, Category = "Figma2UMG", AdvancedDisplay, meta = (EditCondition = "HasCondition"))
	EOverrideConditionCheck StringCheckType = EOverrideConditionCheck::Contains;

	UPROPERTY(EditAnywhere, Category = "Figma2UMG", AdvancedDisplay, meta = (EditCondition = "HasCondition"))
	FString NameComparison = FString("Figma node name");

};

USTRUCT()
struct FIGMA2UMG_API FBorderOverride : public FWidgetOverride
{
	GENERATED_BODY()
public:
	FBorderOverride() = default;

	UPROPERTY(EditAnywhere, Category = "Figma2UMG")
	TSubclassOf<UBorder> ClassOverride = UBorder::StaticClass();
};

USTRUCT()
struct FIGMA2UMG_API FButtonOverride : public FWidgetOverride
{
	GENERATED_BODY()
public:
	FButtonOverride() = default;

	UPROPERTY(EditAnywhere, Category = "Figma2UMG")
	TSubclassOf<UButton> ClassOverride = UButton::StaticClass();
};

USTRUCT()
struct FIGMA2UMG_API FCanvasPanelOverride : public FWidgetOverride
{
	GENERATED_BODY()
public:
	FCanvasPanelOverride() = default;

	UPROPERTY(EditAnywhere, Category = "Figma2UMG")
	TSubclassOf<UCanvasPanel> ClassOverride = UCanvasPanel::StaticClass();
};

USTRUCT()
struct FIGMA2UMG_API FImageOverride : public FWidgetOverride
{
	GENERATED_BODY()
public:
	FImageOverride() = default;

	UPROPERTY(EditAnywhere, Category = "Figma2UMG")
	TSubclassOf<UImage> ClassOverride = UImage::StaticClass();
};

USTRUCT()
struct FIGMA2UMG_API FHorizontalBoxOverride : public FWidgetOverride
{
	GENERATED_BODY()
public:
	FHorizontalBoxOverride() = default;

	UPROPERTY(EditAnywhere, Category = "Figma2UMG")
	TSubclassOf<UHorizontalBox> ClassOverride = UHorizontalBox::StaticClass();
};

USTRUCT()
struct FIGMA2UMG_API FVerticalBoxOverride : public FWidgetOverride
{
	GENERATED_BODY()
public:
	FVerticalBoxOverride() = default;

	UPROPERTY(EditAnywhere, Category = "Figma2UMG")
	TSubclassOf<UVerticalBox> ClassOverride = UVerticalBox::StaticClass();
};

USTRUCT()
struct FIGMA2UMG_API FSizeBoxOverride : public FWidgetOverride
{
	GENERATED_BODY()
public:
	FSizeBoxOverride() = default;

	UPROPERTY(EditAnywhere, Category = "Figma2UMG")
	TSubclassOf<USizeBox> ClassOverride = USizeBox::StaticClass();
};

USTRUCT()
struct FIGMA2UMG_API FTextBlockOverride : public FWidgetOverride
{
	GENERATED_BODY()
public:
	FTextBlockOverride() = default;

	UPROPERTY(EditAnywhere, Category = "Figma2UMG")
	TSubclassOf<UTextBlock> ClassOverride = UTextBlock::StaticClass();
};

USTRUCT()
struct FIGMA2UMG_API FWidgetSwitcherOverride : public FWidgetOverride
{
	GENERATED_BODY()
public:
	FWidgetSwitcherOverride() = default;

	UPROPERTY(EditAnywhere, Category = "Figma2UMG")
	TSubclassOf<UWidgetSwitcher> ClassOverride = UWidgetSwitcher::StaticClass();
};

USTRUCT()
struct FIGMA2UMG_API FWrapBoxOverride : public FWidgetOverride
{
	GENERATED_BODY()
public:
	FWrapBoxOverride() = default;

	UPROPERTY(EditAnywhere, Category = "Figma2UMG")
	TSubclassOf<UWrapBox> ClassOverride = UWrapBox::StaticClass();
};

USTRUCT()
struct FIGMA2UMG_API FClassOverrides
{
	GENERATED_BODY()
public:
	FClassOverrides() = default;

	UPROPERTY(EditAnywhere, Category = "Figma2UMG")
	TArray<FBorderOverride> BorderRules;

	UPROPERTY(EditAnywhere, Category = "Figma2UMG")
	TArray<FButtonOverride> ButtonRules;

	UPROPERTY(EditAnywhere, Category = "Figma2UMG")
	TArray<FCanvasPanelOverride> CanvasPanelRules;

	UPROPERTY(EditAnywhere, Category = "Figma2UMG")
	TArray<FImageOverride> ImageRules;

	UPROPERTY(EditAnywhere, Category = "Figma2UMG")
	TArray<FHorizontalBoxOverride> HorizontalBoxRules;

	UPROPERTY(EditAnywhere, Category = "Figma2UMG")
	TArray<FVerticalBoxOverride> VerticalBoxRules;

	UPROPERTY(EditAnywhere, Category = "Figma2UMG")
	TArray<FSizeBoxOverride> SizeBoxRules;

	UPROPERTY(EditAnywhere, Category = "Figma2UMG")
	TArray<FTextBlockOverride> TextBlockRules;

	UPROPERTY(EditAnywhere, Category = "Figma2UMG")
	TArray<FWidgetSwitcherOverride> WidgetSwitcherRules;

	UPROPERTY(EditAnywhere, Category = "Figma2UMG")
	TArray<FWrapBoxOverride> WrapBoxRules;

};