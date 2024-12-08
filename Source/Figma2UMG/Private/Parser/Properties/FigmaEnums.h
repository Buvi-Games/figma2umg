// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "FigmaEnums.generated.h"

UENUM()
enum class ENodeTypes
{
	DOCUMENT,
	CANVAS,
	FRAME,
	GROUP,
	SECTION,
	VECTOR,
	BOOLEAN_OPERATION,
	STAR,
	LINE,
	ELLIPSE,
	REGULAR_POLYGON,
	RECTANGLE,
	TABLE,
	TABLE_CELL,
	TEXT,
	SLICE,
	COMPONENT,
	COMPONENT_SET,
	INSTANCE,
	STICKY,
	SHAPE_WITH_TEXT,
	CONNECTOR,
	WASHI_TAPE,
};

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
enum class EFigmaLayoutConstraintVertical
{
	TOP,
	BOTTOM,
	CENTER,
	TOP_BOTTOM,
	SCALE,
};

UENUM()
enum class EFigmaLayoutConstraintHorizontal
{
	LEFT,
	RIGHT,
	CENTER,
	LEFT_RIGHT,
	SCALE,
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

UENUM()
enum class EFigmaComponentPropertyType
{
	BOOLEAN,
	INSTANCE_SWAP,
	TEXT,
	VARIANT,
};

UENUM()
enum class EFigmaTriggerType
{
	ON_CLICK,
	ON_HOVER,
	ON_PRESS,
	ON_DRAG,
	AFTER_TIMEOUT,
	MOUSE_ENTER,
	MOUSE_LEAVE,
	MOUSE_UP,
	MOUSE_DOWN,
	ON_MEDIA_END,
	ON_KEY_DOWN,
	ON_KEY_UP,
	ON_MEDIA_HIT,
};

UENUM()
enum class EFigmaInputDevice
{
	KEYBOARD,
	XBOX_ONE,
	PS4,
	SWITCH_PRO,
	UNKNOWN_CONTROLLER,
};

UENUM()
enum class EFigmaActionType
{
	BACK,
	CLOSE,
	URL,
	UPDATE_MEDIA_RUNTIME,
	SET_VARIABLE,
	SET_VARIABLE_MODE,
	CONDITIONAL,
	NODE,
};

UENUM()
enum class EFigmaActionNodeNavigation
{
	NAVIGATE,
	SWAP,
	OVERLAY,
	SCROLL_TO,
	CHANGE_TO,
};

UENUM()
enum class EFigmaActionMedia
{
	PLAY,
	PAUSE,
	TOGGLE_PLAY_PAUSE,
	MUTE,
	UNMUTE,
	TOGGLE_MUTE_UNMUTE,
	SKIP_FORWARD,
	SKIP_BACKWARD,
	SKIP_TO,
};

UENUM()
enum class EFigmaTransitionType
{
	DISSOLVE,
	SMART_ANIMATE,
	SCROLL_ANIMATE,
	MOVE_IN,
	MOVE_OUT,
	PUSH,
	SLIDE_IN,
	SLIDE_OUT
};

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