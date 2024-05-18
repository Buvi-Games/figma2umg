// Copyright 2024 Buvi Games. All Rights Reserved.


void FFigmaTypeStyle::PostSerialize(const TSharedPtr<FJsonObject> JsonObj)
{
	if (JsonObj.IsValid() && JsonObj->HasTypedField<EJson::Array>("fills"))
	{
		const TArray<TSharedPtr<FJsonValue>>& ArrayJson = JsonObj->GetArrayField("fills");
		for (int i = 0; i < ArrayJson.Num() && Fills.Num(); i++)
		{
			const TSharedPtr<FJsonValue>& Item = ArrayJson[i];
			if (Item.IsValid() && Item->Type == EJson::Object)
			{
				const TSharedPtr<FJsonObject>& ItemObject = Item->AsObject();
				Fills[i].PostSerialize(ItemObject);
			}
		}
	}
}
