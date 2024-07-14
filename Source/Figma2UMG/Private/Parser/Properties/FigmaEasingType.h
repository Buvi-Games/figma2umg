// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "FigmaEasingType.generated.h"

UENUM()
enum class EFigmaEasingType
{
	//This type is a string enum with the following possible values
	LINEAR,// : No easing, similar to CSS linear.
	EASE_IN,// : Ease in with an animation curve similar to CSS ease - in.
	EASE_OUT,// : Ease out with an animation curve similar to CSS ease - out.
	EASE_IN_AND_OUT,// : Ease in and then out with an animation curve similar to CSS ease - in - out.
	GENTLE_SPRING,// : Gentle spring animation similar to react - spring.
};