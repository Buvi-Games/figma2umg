// Copyright 2024 Buvi Games. All Rights Reserved.

#include "FigmaPaint.h"

#include "Figma2UMGModule.h"
#include "Builder/Asset/MaterialBorderBuilder.h"
#include "Builder/Asset/MaterialBuilder.h"
#include "Builder/Asset/Texture2DBuilder.h"
#include "Parser/Nodes/FigmaNode.h"
#include "Materials/MaterialInstanceConstant.h"

void FFigmaPaint::PostSerialize(const TSharedPtr<FJsonObject> JsonObj)
{
	static FString ImageTransformStr("imageTransform");
	if (JsonObj.IsValid() && JsonObj->HasTypedField<EJson::Array>(ImageTransformStr))
	{
		const TArray<TSharedPtr<FJsonValue>>& ArrayJson = JsonObj->GetArrayField(ImageTransformStr);
		for (int i = 0; i < ArrayJson.Num(); i++)
		{
			const TSharedPtr<FJsonValue>& ItemLine = ArrayJson[i];
			if (ItemLine.IsValid() && ItemLine->Type == EJson::Array)
			{
				const TArray<TSharedPtr<FJsonValue>>& LineArrayJson = ItemLine->AsArray();
				for (int j = 0; j < LineArrayJson.Num(); j++)
				{
					const TSharedPtr<FJsonValue>& Item = LineArrayJson[i];
					if (Item.IsValid() && Item->Type == EJson::Number)
					{
						ImageTransform.Matrix.M[i][j] = Item->AsNumber();
					}
				}
			}
		}
	}
}

void FFigmaPaint::CreateAssetBuilder(const FString& InFileKey, const UFigmaNode* OwnerNode, TArray<TScriptInterface<IAssetBuilder>>& AssetBuilders, bool IsStroke /*= false*/)
{
	switch (Type)
	{
	case EPaintTypes::SOLID:
		if(IsStroke)
		{
			UMaterialBorderBuilder* MaterialBuilder = NewObject<UMaterialBorderBuilder>();
			MaterialBuilder->SetNode(InFileKey, OwnerNode);
			MaterialBuilder->SetPaint(this);
			AssetBuilder = MaterialBuilder;
			AssetBuilders.Add(MaterialBuilder);
		}
		break;
	case EPaintTypes::GRADIENT_LINEAR:
	case EPaintTypes::GRADIENT_RADIAL:
	case EPaintTypes::GRADIENT_ANGULAR:
	case EPaintTypes::GRADIENT_DIAMOND:
	{
		UMaterialBuilder* MaterialBuilder = NewObject<UMaterialBuilder>();
		MaterialBuilder->SetNode(InFileKey, OwnerNode);
		MaterialBuilder->SetPaint(this);
		AssetBuilder = MaterialBuilder;
		AssetBuilders.Add(MaterialBuilder);
	}
	break;
	case EPaintTypes::IMAGE:
		UE_LOG_Figma2UMG(Warning, TEXT("[CreatePaintAssetBuilderIfNeeded] Node %s - Paint.Type IMAGE is not supported."), *OwnerNode->GetUniqueName());
		break;
	case EPaintTypes::EMOJI:
		UE_LOG_Figma2UMG(Warning, TEXT("[CreatePaintAssetBuilderIfNeeded] Node %s - Paint.Type EMOJI is not supported."), *OwnerNode->GetUniqueName());
		break;
	case EPaintTypes::VIDEO:
		UE_LOG_Figma2UMG(Warning, TEXT("[CreatePaintAssetBuilderIfNeeded] Node %s - Paint.Type VIDEO is not supported."), *OwnerNode->GetUniqueName());
		break;
	}
}

TObjectPtr<UTexture2D> FFigmaPaint::GetTexture() const
{
	if (const UTexture2DBuilder* Texture2DBuilder = AssetBuilder ? Cast<UTexture2DBuilder>(AssetBuilder.GetObject()) : nullptr)
	{
		return Texture2DBuilder->GetAsset();
	}
	return nullptr;
}

TObjectPtr<UMaterialInterface> FFigmaPaint::GetMaterial() const
{
	if (const UMaterialBorderBuilder* MaterialBorderBuilder = AssetBuilder ? Cast<UMaterialBorderBuilder>(AssetBuilder.GetObject()) : nullptr)
	{
		return MaterialBorderBuilder->GetInstanceAsset();
	}
	else if (const UMaterialBuilder* MaterialBuilder = AssetBuilder ? Cast<UMaterialBuilder>(AssetBuilder.GetObject()) : nullptr)
	{
		return MaterialBuilder->GetAsset();
	}
	return nullptr;
}
