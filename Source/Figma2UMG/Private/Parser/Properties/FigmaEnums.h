// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FigmaEnums.generated.h"

UENUM()
enum class EFigmaLayoutMode
{
	NONE,
	HORIZONTAL,
	VERTICAL,
};

UENUM()
enum class EFigmaLayoutSizing
{
	FIXED,
	HUG,
	FILL,
};

UENUM()
enum class EFigmaLayoutWrap
{
	NO_WRAP,
	WRAP,
};

UENUM()
enum class EFigmaAxisSizingMode
{
	AUTO,
	FIXED,
};

UENUM()
enum class EFigmaPrimaryAxisAlignItems
{
	MIN,
	CENTER,
	MAX,
	SPACE_BETWEEN
};

UENUM()
enum class EFigmaCounterAxisAlignItems
{
	MIN,
	CENTER,
	MAX,
	BASELINE
};

UENUM()
enum class EFigmaCounterAxisAlignContent
{
	AUTO,
	SPACE_BETWEEN
};

UENUM()
enum class EFigmaLayoutPositioning
{
	AUTO,
	ABSOLUTE,
};

UENUM()
enum class EFigmaOverflowDirection
{
	NONE,
	HORIZONTAL_SCROLLING,
	VERTICAL_SCROLLING,
	HORIZONTAL_AND_VERTICAL_SCROLLING,
};

UENUM()
enum class EFigmaLayoutAlign
{
	INHERIT,
	STRETCH,
	MIN,
	CENTER,
	MAX,
};

UENUM()
enum class EFigmaStrokeCap
{
	NONE,
	ROUND,
	SQUARE,
	LINE_ARROW,
	TRIANGLE_ARROW,
	DIAMOND_FILLED,
	CIRCLE_FILLED,
	TRIANGLE_FILLED,
	WASHI_TAPE_1,
	WASHI_TAPE_2,
	WASHI_TAPE_3,
	WASHI_TAPE_4,
	WASHI_TAPE_5,
	WASHI_TAPE_6,
};

UENUM()
enum class EFigmaStrokeJoin
{
	MITER,
	BEVEL,
	ROUND,
};

UENUM()
enum class EFigmaStrokeAlign
{
	INSIDE, // stroke drawn inside the shape boundary,
	OUTSIDE, // stroke drawn outside the shape boundary
	CENTER, // stroke drawn centered along the shape boundary
};


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

UENUM()
enum class EPaintTypes
{
	SOLID,
	GRADIENT_LINEAR,
	GRADIENT_RADIAL,
	GRADIENT_ANGULAR,
	GRADIENT_DIAMOND,
	IMAGE,
	EMOJI,
	VIDEO,
};

UENUM()
enum class EScaleMode
{
	FILL,
	FIT,
	TILE,
	STRETCH,
};