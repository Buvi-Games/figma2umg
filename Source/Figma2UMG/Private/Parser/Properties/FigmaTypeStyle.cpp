// MIT License
// Copyright (c) 2024 Buvi Games

#include "Parser/Properties/FigmaTypeStyle.h"

#include "Dom/JsonObject.h"
#include "Serialization/JsonTypes.h"

void FFigmaTypeStyle::PostSerialize(const TSharedPtr<FJsonObject> JsonObj)
{
	static FString FillsStr("fills");
	if (JsonObj.IsValid() && JsonObj->HasTypedField<EJson::Array>(FillsStr))
	{
		const TArray<TSharedPtr<FJsonValue>>& ArrayJson = JsonObj->GetArrayField(FillsStr);
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

FString FFigmaTypeStyle::GetFaceName() const
{
	static const FString ThinStr = FString("Thin");
	static const FString ExtraLightStr = FString("ExtraLight");
	static const FString LightStr = FString("Light");
	static const FString RegularStr = FString("Regular");
	static const FString MediumStr = FString("Medium");
	static const FString SemiBoldStr = FString("SemiBold");
	static const FString BoldStr = FString("Bold");
	static const FString ExtraBoldStr = FString("ExtraBold");
	static const FString BlackStr = FString("Black");
	static const FString ItalicStr = FString("Italic");

	FString BoldName = RegularStr;
	if (FontWeight == 100)
	{
		BoldName = ThinStr;
	}
	else if (FontWeight == 200)
	{
		BoldName = ExtraLightStr;
	}
	else if (FontWeight == 300)
	{
		BoldName = LightStr;
	}
	else if (FontWeight == 400)
	{
		BoldName = RegularStr;
		if(Italic)
		{
			return ItalicStr;
		}
	}
	else if (FontWeight == 500)
	{
		BoldName = MediumStr;
	}
	else if (FontWeight == 600)
	{
		BoldName = SemiBoldStr;
	}
	else if (FontWeight == 700)
	{
		BoldName = BoldStr;
	}
	else if (FontWeight == 800)
	{
		BoldName = ExtraBoldStr;
	}
	else if (FontWeight == 900)
	{
		BoldName = BlackStr;
	}
	return Italic ? BoldName + ItalicStr : BoldName;
}
