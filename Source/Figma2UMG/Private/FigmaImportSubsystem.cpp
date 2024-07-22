// Copyright 2024 Buvi Games. All Rights Reserved.

#include "FigmaImportSubsystem.h"

#include "Engine/Font.h"
#include "Engine/ObjectLibrary.h"
#include "REST/FigmaImporter.h"
#include "REST/RequestParams.h"


UFigmaImporter* UFigmaImportSubsystem::Request(const TObjectPtr<URequestParams> InProperties, const FOnFigmaImportUpdateStatusCB& InRequesterCallback)
{
	UFigmaImporter* request = Requests.Emplace_GetRef(NewObject<UFigmaImporter>());
	WidgetOverrides = &InProperties->WidgetOverrides;
	FrameToButtonOverride = &InProperties->FrameToButton;

	RefreshFontAssets();
	request->Init(InProperties, InRequesterCallback);;
	request->Run();
	return request;
}

void UFigmaImportSubsystem::RemoveRequest(UFigmaImporter* FigmaImporter)
{
	WidgetOverrides = nullptr;
	FrameToButtonOverride = nullptr;
	Requests.Remove(FigmaImporter);
}

bool UFigmaImportSubsystem::ShouldGenerateButton(const FString& NodeName) const
{
	if (!FrameToButtonOverride)
		return false;

	if (NodeName.IsEmpty())
		return false;

	for (const FWidgetOverride& Override : FrameToButtonOverride->Rules)
	{
		if (Override.Match(NodeName))
			return true;
	}

	return false;
}

void UFigmaImportSubsystem::RefreshFontAssets()
{
	if(FontObjectLibrary)
	{
		FontObjectLibrary->ClearLoaded();
	}
	else
	{
		FontObjectLibrary = UObjectLibrary::CreateLibrary(UFont::StaticClass(), false, GIsEditor);
	}

	FontObjectLibrary->LoadAssetDataFromPath(TEXT("/Game"));
	FontObjectLibrary->LoadAssetDataFromPath(TEXT("/Script/Engine.Font"));
}

UFont* UFigmaImportSubsystem::FindFontAssetFromFamily(const FString& FamilyName) const
{
	TArray<FAssetData> AssetDatas;
	FontObjectLibrary->GetAssetDataList(AssetDatas);

	const FString FontFamily = FamilyName.Replace(TEXT(" "), TEXT(""));
	for (const FAssetData& AssetData : AssetDatas)
	{
		UObject* Asset = AssetData.GetAsset();
		UFont* Font = Cast<UFont>(Asset);
		if (Font && Font->GetName().Equals(FontFamily, ESearchCase::IgnoreCase))
		{
			return Font;
		}
	}

	return nullptr;
}

FGFontFamilyInfo* UFigmaImportSubsystem::FindGoogleFontsInfo(const FString& FamilyName)
{
	FGFontFamilyInfo* GFontFamilyInfo = GoogleFontsInfo.FindByPredicate([FamilyName](const FGFontFamilyInfo& GFontFamilyInfo)
		{
			return GFontFamilyInfo.Family.Equals(FamilyName, ESearchCase::IgnoreCase);
		});

	return GFontFamilyInfo;
}

void UFigmaImportSubsystem::TryRenameWidget(const FString& InName, TObjectPtr<UWidget> Widget)
{
	if (!Widget)
		return;

	if (Widget->GetName().Contains(InName, ESearchCase::IgnoreCase))
		return;

	const FString UniqueName = MakeUniqueObjectName(Widget->GetOuter(), Widget->GetClass(), *InName).ToString();
	Widget->Rename(*UniqueName);
}
