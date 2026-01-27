// MIT License
// Copyright (c) 2024 Buvi Games

#include "FigmaImportSubsystem.h"

#include "Figma2UMGModule.h"
#include "PackageTools.h"
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
	NewFonts.Reset();
	if(FontObjectLibrary)
	{
		FontObjectLibrary->ClearLoaded();
	}
	else
	{
		FontObjectLibrary = UObjectLibrary::CreateLibrary(UFont::StaticClass(), false, GIsEditor);
	}

	TArray<FString> Paths;
	Paths.Add(TEXT("/Game"));
	Paths.Add(TEXT("/Engine/EngineFonts"));
	FontObjectLibrary->LoadAssetDataFromPaths(Paths);
}

void UFigmaImportSubsystem::AddNewFont(UFont* NewFont)
{
	NewFonts.AddUnique(NewFont);
}

UFont* UFigmaImportSubsystem::FindFontAssetFromFamily(const FString& FamilyName) const
{
	TArray<FAssetData> AssetDatas;
	FontObjectLibrary->GetAssetDataList(AssetDatas);

	const FString FontFamily = UPackageTools::SanitizePackageName(FamilyName.Replace(TEXT(" "), TEXT("")));
	const FString FontFamilyWithSuffix = FontFamily + TEXT("_Font");

	for (const FAssetData& AssetData : AssetDatas)
	{
		const FString AssetName = AssetData.AssetName.ToString();
		// Check for exact match or match with _Font suffix (UE naming convention)
		if (!FontFamily.Equals(AssetName, ESearchCase::IgnoreCase) &&
		    !FontFamilyWithSuffix.Equals(AssetName, ESearchCase::IgnoreCase))
			continue;

		UObject* Asset = AssetData.GetAsset();
		UFont* Font = Cast<UFont>(Asset);
		if (Font)
		{
			return Font;
		}
	}

	for (UFont* Font : NewFonts)
	{
		if (Font)
		{
			const FString FontName = Font->GetName();
			if (FontName.Equals(FontFamily, ESearchCase::IgnoreCase) ||
			    FontName.Equals(FontFamilyWithSuffix, ESearchCase::IgnoreCase))
			{
				return Font;
			}
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

	const FString CurrentName = Widget->GetName();
	if (CurrentName.Equals(InName, ESearchCase::IgnoreCase))
		return;

	UObject* Outer = Widget->GetOuter();

	// Check if an object with the desired name already exists (excluding this widget)
	UObject* ExistingObject = FindObject<UObject>(Outer, *InName);
	if (ExistingObject != nullptr && ExistingObject != Widget)
	{
		// Move existing object to transient package to free up the name
		ExistingObject->Rename(nullptr, GetTransientPackage(), REN_DontCreateRedirectors | REN_ForceNoResetLoaders);
		ExistingObject->MarkAsGarbage();
	}

	// Use exact name (existing object has been moved out of the way if there was a collision)
	Widget->Rename(*InName);
}

UMaterialInstanceConstant* UFigmaImportSubsystem::GetBorderMaterialInstances(float StrokeWeight) const
{
	if (BorderMaterialInstances.Contains(StrokeWeight))
	{
		return BorderMaterialInstances[StrokeWeight];
	}

	return nullptr;
}

void UFigmaImportSubsystem::AddBorderMaterialInstances(float StrokeWeight, UMaterialInstanceConstant* MaterialInstanceConstant)
{
	BorderMaterialInstances.Add(StrokeWeight, MaterialInstanceConstant);
}

void UFigmaImportSubsystem::ResetBorderMaterials()
{
	BorderMaterial = nullptr;
	BorderMaterialInstances.Reset();
}

void UFigmaImportSubsystem::ImportFromFigma(const FString& FileKey, const TArray<FString>& NodeIds, const FString& OutputFolder)
{
	URequestParams* Params = NewObject<URequestParams>();
	Params->FileKey = FileKey;
	Params->Ids = NodeIds;
	if (!OutputFolder.IsEmpty())
	{
		Params->ContentRootFolder = OutputFolder;
	}

	// Reset state for new import
	bImportInProgress = true;
	bLastImportSucceeded = false;
	bLastImportRateLimited = false;
	RetryAfterSeconds = 60;
	LastImportMessage = TEXT("Import in progress...");

	UE_LOG(LogFigma2UMG, Warning, TEXT("=== FIGMA2UMG BUILD VERSION 2024-01-24-A === C++ CODE LOADED SUCCESSFULLY ==="));
	UE_LOG(LogFigma2UMG, Display, TEXT("Starting Figma import: FileKey=%s, NodeIds=%d, OutputFolder=%s"),
		*FileKey, NodeIds.Num(), *Params->ContentRootFolder);

	Request(Params, FOnFigmaImportUpdateStatusCB::CreateLambda(
		[this](eRequestStatus Status, FString Message) {
			switch (Status)
			{
			case eRequestStatus::Succeeded:
				bImportInProgress = false;
				bLastImportSucceeded = true;
				LastImportMessage = Message;
				UE_LOG(LogFigma2UMG, Display, TEXT("Figma import completed: %s"), *Message);
				break;
			case eRequestStatus::Processing:
				// Still in progress, update message but don't change flags
				LastImportMessage = Message;
				UE_LOG(LogFigma2UMG, Display, TEXT("Figma import progress: %s"), *Message);
				break;
			case eRequestStatus::Failed:
				bImportInProgress = false;
				bLastImportSucceeded = false;
				LastImportMessage = Message;
				UE_LOG(LogFigma2UMG, Error, TEXT("Figma import failed: %s"), *Message);
				break;
			default:
				LastImportMessage = Message;
				UE_LOG(LogFigma2UMG, Display, TEXT("Figma import status: %s"), *Message);
				break;
			}
		}));
}

FString UFigmaImportSubsystem::GetUniqueWidgetName(TMap<FString, int32>& NameTracker, const FString& BaseName)
{
	// Get or create the count for this base name
	int32& Count = NameTracker.FindOrAdd(BaseName);

	FString Result;
	if (Count == 0)
	{
		// First use of this name: return as-is (e.g., "Fill")
		Result = BaseName;
	}
	else
	{
		// Subsequent uses: append suffix (e.g., "Fill_2", "Fill_3")
		Result = FString::Printf(TEXT("%s_%d"), *BaseName, Count + 1);
	}

	// Increment the count for next time
	Count++;

	return Result;
}
