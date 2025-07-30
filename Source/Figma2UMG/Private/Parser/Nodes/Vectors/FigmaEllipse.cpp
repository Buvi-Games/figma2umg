// MIT License
// Copyright (c) 2024 Buvi Games


#include "Parser/Nodes/Vectors/FigmaEllipse.h"

#include "Builder/Widget/ImageWidgetBuilder.h"

FVector2D UFigmaEllipse::GetAbsolutePosition(const bool IsTopWidgetForNode) const
{
	return AbsoluteBoundingBox.GetPosition(IsTopWidgetForNode ? GetAbsoluteRotation() : 0.0f);
}

FVector2D UFigmaEllipse::GetAbsoluteSize(const bool IsTopWidgetForNode) const
{
	return AbsoluteBoundingBox.GetSize(IsTopWidgetForNode ? GetAbsoluteRotation() : 0.0f);	
}

FVector2D UFigmaEllipse::GetAbsoluteCenter() const
{
	return AbsoluteBoundingBox.GetCenter();
}

bool UFigmaEllipse::CreateAssetBuilder(const FString& InFileKey, TArray<TScriptInterface<IAssetBuilder>>& AssetBuilders)
{
	if(AbsoluteBoundingBox.Height != AbsoluteBoundingBox.Width)
	{
		return Super::CreateAssetBuilder(InFileKey, AssetBuilders);
	}
	else
	{
		for (const FFigmaPaint& Fill : Fills)
		{
			if(Fill.Type != EPaintTypes::SOLID)
			{
				return Super::CreateAssetBuilder(InFileKey, AssetBuilders);
			}
		}
		for (const FFigmaPaint& Stroke : Strokes)
		{
			if (Stroke.Type != EPaintTypes::SOLID)
			{
				return Super::CreateAssetBuilder(InFileKey, AssetBuilders);
			}
		}

		return false;
	}
}

TScriptInterface<IWidgetBuilder> UFigmaEllipse::CreateWidgetBuilders(bool IsRoot, bool AllowFrameButton) const
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
			if (Paint.Type == EPaintTypes::SOLID)
			{
				FoundColor = true;
				SolidColor = Paint.GetLinearColor();
			}
		}

		if (Fills.IsEmpty())
		{
			for (const FFigmaPaint& Paint : Strokes)
			{
				if (Paint.Type == EPaintTypes::SOLID)
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

bool UFigmaEllipse::DoesSupportImageRef() const
{
	// Only Circle ca support by doing RoundCorners
	return (AbsoluteBoundingBox.Height == AbsoluteBoundingBox.Width);
}
