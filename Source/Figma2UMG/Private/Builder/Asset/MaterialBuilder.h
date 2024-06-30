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
	virtual UPackage* GetAssetPackage() const override;

	const TObjectPtr<UMaterial>& GetAsset() const;

	void SetPaint(const FFigmaPaint* InPaint);
protected:
	void Setup() const;
	UMaterialExpression* SetupGradientInput(UMaterialGraph* ExpressionGraph) const;
	UMaterialExpression* SetupGradientNode(UMaterialExpression* PositionInput) const;
	UMaterialExpression* SetupGradientLinearNode(UMaterialExpression* PositionInput) const;

	UPROPERTY()
	TObjectPtr<UMaterial> Asset = nullptr;

	const FFigmaPaint* Paint = nullptr;
};
