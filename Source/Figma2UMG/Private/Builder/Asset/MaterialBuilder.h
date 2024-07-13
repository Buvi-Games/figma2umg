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
	bool HasAlpha() const;

	UMaterialExpression* SetupGradientInput(int& OutputIndex) const;
	UMaterialExpression* SetupLinearGradientInput(int& OutputIndex) const;
	UMaterialExpression* SetupLinearGradientCustomInput(UMaterialExpressionMaterialFunctionCall* LinearGradientExpression) const;
	UMaterialExpression* SetupRadialGradientInput(int& OutputIndex) const;
	UMaterialExpression* SetupAngularGradientInput(int& OutputIndex) const;

	UMaterialExpression* SetupColorExpression(UMaterialExpression* PositionInput, const int OutputIndex) const;
	UMaterialExpression* SetupGradientColorExpression(UMaterialExpression* PositionInput, const int OutputIndex) const;

	UMaterialExpression* SetupMaskExpression(UMaterialExpression* InputExpression, uint32 R, uint32 G, uint32 B, uint32 A, float NodePosX, float NodePosY) const;
	UMaterialExpression* SetupUVInputExpression(float NodePosX = -1800.0f) const;
	UMaterialExpressionMaterialFunctionCall* SetupMaterialFunction(const FString& FunctionPath, float NodePosX = -1200.0f) const;
	UMaterialExpression* InvertOutput(UMaterialExpression* OutputExpression, const int OutputIndex) const;

	UPROPERTY()
	TObjectPtr<UMaterial> Asset = nullptr;

	const FFigmaPaint* Paint = nullptr;
};
