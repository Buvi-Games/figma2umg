// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetBuilder.h"
#include "MaterialEditingLibrary.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "Parser/Properties/FigmaPaint.h"
#include "MaterialBuilder.generated.h"

class UMaterialExpressionConstant;

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
	virtual void Setup() const;
	virtual bool HasAlpha() const;

	UMaterialExpression* SetupGradientInput(int& OutputIndex) const;
	UMaterialExpression* SetupLinearGradientInput(int& OutputIndex) const;
	UMaterialExpression* SetupLinearGradientCustomInput(UMaterialExpressionMaterialFunctionCall* LinearGradientExpression) const;
	UMaterialExpression* SetupRadialGradientInput(int& OutputIndex) const;
	UMaterialExpression* SetupAngularGradientInput(int& OutputIndex) const;
	UMaterialExpression* SetupDiamondGradientInput(int& OutputIndex) const;

	UMaterialExpression* SetupColorExpression(UMaterialExpression* PositionInput, const int OutputIndex) const;
	UMaterialExpression* SetupGradientColorExpression(UMaterialExpression* PositionInput, const int OutputIndex) const;

	UMaterialExpression* SetupMaskExpression(UMaterialExpression* InputExpression, uint32 R, uint32 G, uint32 B, uint32 A, float NodePosX, float NodePosY) const;
	UMaterialExpression* SetupUVInputExpression(float NodePosX = -1800.0f) const;
	UMaterialExpressionMaterialFunctionCall* SetupMaterialFunction(const FString& FunctionPath, float NodePosX = -1200.0f, float NodePosY = 0.0f) const;
	UMaterialExpression* InvertOutput(UMaterialExpression* OutputExpression, const int OutputIndex) const;

	UMaterialExpressionConstant* AddConstant(float ConstValue, UMaterialExpression* InputExpression, const int InputIndex) const;

	template <class ParameterT, class ValueT>
	ParameterT* AddParameter(const FName& ParameterName, const ValueT& DefaultValue, float NodePosX, float NodePosY) const;
	template<class OperationT>
	UMaterialExpression* Operation(UMaterialExpression* AExpression, const int AOutputIndex, UMaterialExpression* BExpression, const int BOutputIndex, const float AConstValue = 1.0f, const float BConstValue = 1.0f, const float OffsetX = 200.0f, const float OffsetY = 0.0f) const;

	UPROPERTY()
	TObjectPtr<UMaterial> Asset = nullptr;

	const FFigmaPaint* Paint = nullptr;
};

template <class ParameterT, class ValueT>
ParameterT* UMaterialBuilder::AddParameter(const FName& ParameterName, const ValueT& DefaultValue, float NodePosX, float NodePosY) const
{
	ParameterT* Param = nullptr;
	for (UMaterialExpression* Expression : Asset->GetExpressions())
	{
		ParameterT* MaterialExpression = Cast<ParameterT>(Expression);
		if (MaterialExpression && MaterialExpression->ParameterName == ParameterName)
		{
			Param = MaterialExpression;
			break;
		}
	}

	if (!Param)
	{
		Param = Cast<ParameterT>(UMaterialEditingLibrary::CreateMaterialExpressionEx(Asset, nullptr, ParameterT::StaticClass(), Asset, NodePosX, NodePosY));
		Param->ParameterName = ParameterName;
	}

	Param->DefaultValue = DefaultValue;

	return Param;
}

template <class OperationT>
UMaterialExpression* UMaterialBuilder::Operation(UMaterialExpression* AExpression, const int AOutputIndex, UMaterialExpression* BExpression, const int BOutputIndex, const float AConstValue, const float BConstValue, const float OffsetX, const float OffsetY) const
{
	OperationT* OperationExpression = nullptr;
	if(!AExpression && !BExpression)
		return nullptr;

	for (UMaterialExpression* Expression : Asset->GetExpressions())
	{
		OperationT* MaterialExpressionMultiply = Cast<OperationT>(Expression);
		if (MaterialExpressionMultiply && MaterialExpressionMultiply->A.Expression == AExpression && MaterialExpressionMultiply->B.Expression == BExpression)
		{
			OperationExpression = MaterialExpressionMultiply;
			break;
		}
	}

	if (!OperationExpression)
	{
		;
		FVector2D NodePosition;
		if (AExpression && BExpression)
		{
			NodePosition.X = FMath::Max(AExpression->MaterialExpressionEditorX, BExpression->MaterialExpressionEditorX);
			NodePosition.Y = (AExpression->MaterialExpressionEditorY + BExpression->MaterialExpressionEditorY) * 0.5f;
		}
		else if(AExpression)
		{
			NodePosition.X = AExpression->MaterialExpressionEditorX;
			NodePosition.Y = AExpression->MaterialExpressionEditorY;
		}
		else if (BExpression)
		{
			NodePosition.X = BExpression->MaterialExpressionEditorX;
			NodePosition.Y = BExpression->MaterialExpressionEditorY;
		}

		NodePosition.X += OffsetX;
		NodePosition.Y += OffsetY;

		OperationExpression = Cast<OperationT>(UMaterialEditingLibrary::CreateMaterialExpressionEx(Asset, nullptr, OperationT::StaticClass(), Asset, NodePosition.X, NodePosition.Y));
		if (AExpression)
		{
			OperationExpression->A.Connect(AOutputIndex, AExpression);
		}
		if (BExpression)
		{
			OperationExpression->B.Connect(BOutputIndex, BExpression);
		}
	}

	if (!AExpression)
	{
		OperationExpression->ConstA = AConstValue;
	}
	if (!BExpression)
	{
		OperationExpression->ConstB = BConstValue;
	}

	return OperationExpression;
}
