// Copyright 2024 Buvi Games. All Rights Reserved.

#include "FigmaPaint.h"

void FFigmaPaint::PostSerialize(const TSharedPtr<FJsonObject> JsonObj)
{
	if (JsonObj.IsValid() && JsonObj->HasTypedField<EJson::Array>("imageTransform"))
	{
		const TArray<TSharedPtr<FJsonValue>>& ArrayJson = JsonObj->GetArrayField("imageTransform");
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
