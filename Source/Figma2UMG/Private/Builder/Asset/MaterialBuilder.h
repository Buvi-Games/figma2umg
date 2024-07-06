// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetBuilder.h"
#include "Parser/Properties/FigmaPaint.h"
#include "MaterialBuilder.generated.h"

UCLASS()
class UMaterialBuilder : public UObject, public IAssetBuilder
{
	GENERATED_BODY()
public:
	virtual void LoadOrCreateAssets() override;
	virtual void LoadAssets() override;
	virtual void Reset() override;
	virtual UPackage* GetAssetPackage() const override;

	const TObjectPtr<UMaterial>& GetAsset() const;

	void SetPaint(const FFigmaPaint* InPaint);
protected:
	void Setup() const;
	UMaterialExpression* SetupGradientInput(int& OutputIndex) const;
	UMaterialExpressionMaterialFunctionCall* SetupLinearGradientInput(float NodePosX = -1200.0f) const;
	UMaterialExpression* SetupLinearGradientCustomInput(UMaterialExpressionMaterialFunctionCall* LinearGradientExpression) const;

	UMaterialExpression* SetupGradientNode(UMaterialExpression* PositionInput, const int OutputIndex) const;
	UMaterialExpression* SetupGradientLinearNode(UMaterialExpression* PositionInput, const int OutputIndex) const;

	UMaterialExpression* InvertOutput(UMaterialExpression* OutputExpression, const int OutputIndex) const;

	UPROPERTY()
	TObjectPtr<UMaterial> Asset = nullptr;

	const FFigmaPaint* Paint = nullptr;
};
