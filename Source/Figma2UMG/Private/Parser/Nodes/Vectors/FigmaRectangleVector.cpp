// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/Vectors/FigmaRectangleVector.h"

#include "Builder/Widget/ImageWidgetBuilder.h"

FVector2D UFigmaRectangleVector::GetAbsolutePosition(const bool IsTopWidgetForNode) const
{
	return AbsoluteBoundingBox.GetPosition(IsTopWidgetForNode ? GetAbsoluteRotation() : 0.0f);
}

FVector2D UFigmaRectangleVector::GetAbsoluteSize(const bool IsTopWidgetForNode) const
{
	return AbsoluteBoundingBox.GetSize(IsTopWidgetForNode ? GetAbsoluteRotation() : 0.0f);	
}

FVector2D UFigmaRectangleVector::GetAbsoluteCenter() const
{
	return AbsoluteBoundingBox.GetCenter();
}

bool UFigmaRectangleVector::CreateAssetBuilder(const FString& InFileKey, TArray<TScriptInterface<IAssetBuilder>>& AssetBuilders)
{
	bool HasImage = false;
	for (FFigmaPaint& Paint : Fills)
	{
		if (Paint.Type == EPaintTypes::IMAGE)
		{
			HasImage = true;
			break;
		}
	}

	if (HasImage)
	{
		return Super::CreateAssetBuilder(InFileKey, AssetBuilders);
	}
	else
	{
		CreatePaintAssetBuilderIfNeeded(InFileKey, AssetBuilders, Fills, Strokes);
	}
	return true;
}

TScriptInterface<IWidgetBuilder> UFigmaRectangleVector::CreateWidgetBuilders(bool IsRoot, bool AllowFrameButton) const
{
	if (AssetBuilder)
	{
		return Super::CreateWidgetBuilders(IsRoot, AllowFrameButton);
	}
	else
	{
		UImageWidgetBuilder* ImageWidgetBuilder = NewObject<UImageWidgetBuilder>();
		ImageWidgetBuilder->SetNode(this);
		FLinearColor SolidColor;
		bool FoundColor = false;
		for (const FFigmaPaint& Paint : Fills)
		{
			if (const TObjectPtr<UMaterialInterface> Material = Paint.GetMaterial())
			{
				ImageWidgetBuilder->SetMaterial(Material, Paint.GetLinearColor());
				return ImageWidgetBuilder;
			}
			else if(Paint.Type == EPaintTypes::SOLID)
			{
				FoundColor = true;
				SolidColor = Paint.GetLinearColor();
			}
		}

		if(Fills.IsEmpty())
		{
			for (const FFigmaPaint& Paint : Strokes)
			{
				if (const TObjectPtr<UMaterialInterface> Material = Paint.GetMaterial())
				{
					ImageWidgetBuilder->SetMaterial(Material, Paint.GetLinearColor());
					return ImageWidgetBuilder;
				}
				else if (Paint.Type == EPaintTypes::SOLID)
				{
					FoundColor = true;
					SolidColor = Paint.GetLinearColor();
				}
			}
		}

		//No texture, no Material. Set a color.
		if (FoundColor)
		{
			ImageWidgetBuilder->SetColor(SolidColor);
		}
		return ImageWidgetBuilder;
	}
}

bool UFigmaRectangleVector::DoesSupportImageRef() const
{
	return true;
}
