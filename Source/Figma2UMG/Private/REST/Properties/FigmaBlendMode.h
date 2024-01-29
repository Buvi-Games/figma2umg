// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FigmaBlendMode.generated.h"

UENUM()
enum class EFigmaBlendMode
{	
	// This type is a string enum with the following possible values
	// Normal blends:
	PASS_THROUGH,// (only applicable to objects with children)
	NORMAL,

	// Darken:
	DARKEN,
	MULTIPLY,
	LINEAR_BURN,// ("Plus darker" in Figma)
	COLOR_BURN,

	// Lighten:
	LIGHTEN,
	SCREEN,
	LINEAR_DODGE, // ("Plus lighter" in Figma)
	COLOR_DODGE,

	// Contrast:
	OVERLAY,
	SOFT_LIGHT,
	HARD_LIGHT,
	
	// Inversion:
	DIFFERENCE,
	EXCLUSION,
	
	// Component:
	HUE,
	SATURATION,
	COLOR,
	LUMINOSITY
};