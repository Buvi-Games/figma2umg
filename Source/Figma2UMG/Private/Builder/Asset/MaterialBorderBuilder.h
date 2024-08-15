// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MaterialBuilder.h"
#include "MaterialBorderBuilder.generated.h"

UCLASS()
class FIGMA2UMG_API UMaterialBorderBuilder : public UMaterialBuilder
{
	GENERATED_BODY()
public:
	virtual void LoadOrCreateAssets() override;
	virtual void LoadAssets() override;
	virtual void Reset() override;

	virtual void AddPackages(TArray<UPackage*>& Packages) const override;

	TObjectPtr<UMaterialInstanceConstant> GetInstanceAsset() const { return InstanceAsset; }
protected:
	virtual void Setup() const override;
	void SetupInstance() const;
	virtual bool HasAlpha() const override;

	int GetStrokeWeight() const;

	UMaterialExpression* SetupMaterial() const;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceConstant> InstanceAsset = nullptr;

	bool ManageMaterial = false;
	bool ManageMaterialInstance = false;
};

