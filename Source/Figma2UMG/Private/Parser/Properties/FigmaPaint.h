// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FigmaBlendMode.h"
#include "FigmaColor.h"
#include "FigmaColorStop.h"
#include "FigmaEnums.h"
#include "FigmaImageFilters.h"
#include "FigmaTransform.h"
#include "FigmaVariableAlias.h"
#include "FigmaVector.h"

#include "FigmaPaint.generated.h"


class UFigmaNode;
class IAssetBuilder;

USTRUCT()
struct FIGMA2UMG_API FFigmaPaint
{
public:
	GENERATED_BODY()

	void PostSerialize(const TSharedPtr<FJsonObject> JsonObj);

	FLinearColor GetLinearColor() const
	{
		return FLinearColor(Color.R, Color.G, Color.B, Opacity);
	}

	void CreateAssetBuilder(const FString& InFileKey, const UFigmaNode* OwnerNode, TArray<TScriptInterface<IAssetBuilder>>& AssetBuilders);
	TObjectPtr<UTexture2D> GetTexture() const;
	TObjectPtr<UMaterial> GetMaterial() const;

	UPROPERTY()
	EPaintTypes Type;

	UPROPERTY()
	bool Visible = true;

	UPROPERTY()
	float Opacity = 1.0f;

	UPROPERTY()
	FFigmaColor Color;

	UPROPERTY()
	EFigmaBlendMode BlendMode;

	UPROPERTY()
	TArray<FFigmaVector> GradientHandlePositions;

	UPROPERTY()
	TArray<FFigmaColorStop> GradientStops;

	UPROPERTY()
	EScaleMode ScaleMode;

	FFigmaTransform ImageTransform;

	UPROPERTY()
	float ScalingFactor;

	UPROPERTY()
	float Rotation;

	UPROPERTY()
	FString ImageRef;

	UPROPERTY()
	FFigmaImageFilters Filters;

	UPROPERTY()
	FString GifRef;

	UPROPERTY()
	TMap<FString, FFigmaVariableAlias> BoundVariables;

protected:
	TScriptInterface<IAssetBuilder> AssetBuilder = nullptr;
};
