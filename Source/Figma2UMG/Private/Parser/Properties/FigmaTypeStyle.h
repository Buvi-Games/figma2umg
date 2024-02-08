// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FigmaHyperlink.h"

#include "FigmaTypeStyle.generated.h"

UENUM()
enum class EFigmaTextCase
{
	ORIGINAL,
	UPPER,
	LOWER,
	TITLE,
	SMALL_CAPS,
	SMALL_CAPS_FORCED,
};

UENUM()
enum class EFigmaTextDecoration
{
	NONE,
	STRIKETHROUGH,
	UNDERLINE,
};

UENUM()
enum class EFigmaTextAutoResize
{
	NONE,
	HEIGHT,
	WIDTH_AND_HEIGHT,
};

UENUM()
enum class EFigmaTextTruncation
{
	DISABLED,
	ENDING,
};

UENUM()
enum class EFigmaTextAlignHorizontal
{
	LEFT,
	RIGHT,
	CENTER,
	JUSTIFIED,
};

UENUM()
enum class EFigmaTextAlignVertical
{
	TOP,
	CENTER,
	BOTTOM,
};

USTRUCT()
struct FIGMA2UMG_API FFigmaTypeStyle
{
public:
	GENERATED_BODY()

	UPROPERTY()
	FString FontFamily;

	UPROPERTY()
	FString FontPostScriptName;

	UPROPERTY()
	int ParagraphSpacing = 0;

	UPROPERTY()
	int ParagraphIndent = 0;

	UPROPERTY()
	int ListSpacing = 0;

	UPROPERTY()
	bool Italic = false;

	UPROPERTY()
	float FontWeight = 100;

	UPROPERTY()
	int FontSize = 32;

	UPROPERTY()
	EFigmaTextCase TextCase = EFigmaTextCase::ORIGINAL;

	UPROPERTY()
	EFigmaTextDecoration TextDecoration = EFigmaTextDecoration::NONE;

	UPROPERTY()
	EFigmaTextAutoResize TextAutoResize = EFigmaTextAutoResize::NONE;

	UPROPERTY()
	EFigmaTextTruncation TextTruncation = EFigmaTextTruncation::DISABLED;

	UPROPERTY()
	int MaxLinesNumber = -1;

	UPROPERTY()
	EFigmaTextAlignHorizontal TextAlignHorizontal = EFigmaTextAlignHorizontal::LEFT;

	UPROPERTY()
	EFigmaTextAlignVertical TextAlignVertical = EFigmaTextAlignVertical::TOP;

	UPROPERTY()
	int32 LetterSpacing = 0;

	UPROPERTY()
	TArray<FFigmaPaint> Fills;

	UPROPERTY()
	FFigmaHyperlink Hyperlink;

	UPROPERTY()
	TMap<FString, int> OpentypeFlags;

	UPROPERTY()
	float LineHeightPx = 10;

	UPROPERTY()
	float LineHeightPercent = 100.0f;

	UPROPERTY()
	float LineHeightPercentFontSize = 100;

	UPROPERTY()
	FString LineHeightUnit;
	
};
