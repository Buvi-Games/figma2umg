// MIT License
// Copyright (c) 2024 Buvi Games

#pragma once

#include "CoreMinimal.h"
#include "FigmaEnums.h"
#include "FigmaVector.h"

#include "FigmaAction.generated.h"

UCLASS()
class FIGMA2UMG_API UFigmaAction : public UObject
{
public:
	GENERATED_BODY()

	static UFigmaAction* CreateAction(const TSharedPtr<FJsonObject>& ObjectJson);

	UPROPERTY()
	EFigmaActionType Type;
};

UCLASS()
class FIGMA2UMG_API UFigmaBackAction : public UFigmaAction
{
public:
	GENERATED_BODY()
};

UCLASS()
class FIGMA2UMG_API UFigmaCloseAction : public UFigmaAction
{
public:
	GENERATED_BODY()
};

UCLASS()
class FIGMA2UMG_API UFigmaOpenURLAction : public UFigmaAction
{
public:
	GENERATED_BODY()

	UPROPERTY()
	FString URL;
};

UCLASS()
class FIGMA2UMG_API UFigmaUpdateMediaRuntimeAction : public UFigmaAction
{
public:
	GENERATED_BODY()

	UPROPERTY()
	FString DestinationId;

	UPROPERTY()
	EFigmaActionMedia MediaAction;

	UPROPERTY()
	float AmountToSkip = 0.0f;

	UPROPERTY()
	float NewTimestamp = 0.0f;
};

UCLASS()
class FIGMA2UMG_API UFigmaNodeAction : public UFigmaAction
{
public:
	GENERATED_BODY()

	UPROPERTY()
	FString DestinationId;

	UPROPERTY()
	EFigmaActionNodeNavigation Navigation;

	UPROPERTY()
	FString Transition;//ToDo

	UPROPERTY()
	bool PreserveScrollPosition;

	UPROPERTY()
	FFigmaVector OverlayRelativePosition;

	UPROPERTY()
	bool ResetVideoPosition;

	UPROPERTY()
	bool ResetScrollPosition;

	UPROPERTY()
	bool ResetInteractiveComponents;
};

UCLASS()
class FIGMA2UMG_API UFigmaSetVariableAction : public UFigmaAction
{
public:
	GENERATED_BODY()

	UPROPERTY()
	FString VariableId;

	UPROPERTY()
	FString VariableValue;//ToDo
};

UCLASS()
class FIGMA2UMG_API UFigmaSetVariableModeAction : public UFigmaAction
{
public:
	GENERATED_BODY()

	UPROPERTY()
	FString VariableCollectionId;

	UPROPERTY()
	FString VariableModeId;
};

UCLASS()
class FIGMA2UMG_API UFigmaConditionalAction : public UFigmaAction
{
public:
	GENERATED_BODY()

	UPROPERTY()
	FString ConditionalBlocks;//ToDo
};