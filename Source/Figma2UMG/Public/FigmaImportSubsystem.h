// MIT License
// Copyright (c) 2024 Buvi Games

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "Rest/Enums.h"
#include "Settings/ClassOverrides.h"
#include "REST/Gfonts/GFontRequest.h"
#include "Blueprint/WidgetTree.h"

#include "FigmaImportSubsystem.generated.h"

class UObjectLibrary;
struct FClassOverrides;
class UFigmaImporter;
class URequestParams;
class UMaterialInstanceConstant;

DECLARE_DELEGATE_TwoParams(FOnFigmaImportUpdateStatusCB, eRequestStatus, FString);

UCLASS()
class FIGMA2UMG_API UFigmaImportSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()
public:
	UFigmaImporter* Request(const TObjectPtr<URequestParams> InProperties, const FOnFigmaImportUpdateStatusCB& InRequesterCallback);
	void RemoveRequest(UFigmaImporter* FigmaImporter);

	/** Import Figma design(s) programmatically. AccessToken is loaded from config. */
	UFUNCTION(BlueprintCallable, Category = "Figma2UMG")
	void ImportFromFigma(const FString& FileKey, const TArray<FString>& NodeIds, const FString& OutputFolder);

	/** Returns true if an import is currently in progress */
	UFUNCTION(BlueprintCallable, Category = "Figma2UMG")
	bool IsImportInProgress() const { return bImportInProgress; }

	/** Returns true if the last import succeeded */
	UFUNCTION(BlueprintCallable, Category = "Figma2UMG")
	bool DidLastImportSucceed() const { return bLastImportSucceeded; }

	/** Returns the status message from the last import (error message if failed) */
	UFUNCTION(BlueprintCallable, Category = "Figma2UMG")
	FString GetLastImportMessage() const { return LastImportMessage; }

	/** Returns true if the last import failed due to rate limiting */
	UFUNCTION(BlueprintCallable, Category = "Figma2UMG")
	bool WasLastImportRateLimited() const { return bLastImportRateLimited; }

	/** Returns the number of seconds to wait before retrying (from Retry-After header, or default 60) */
	UFUNCTION(BlueprintCallable, Category = "Figma2UMG")
	int32 GetRetryAfterSeconds() const { return RetryAfterSeconds; }

	/** Call this to mark that a rate limit was hit (called by FigmaImporter) */
	void SetRateLimited(int32 InRetryAfterSeconds)
	{
		bLastImportRateLimited = true;
		RetryAfterSeconds = InRetryAfterSeconds > 0 ? InRetryAfterSeconds : 60;
	}

	template <class Type>
	UClass* GetOverrideClassForNode(const FString& NodeName);

	bool ShouldGenerateButton(const FString& NodeName) const;

	void RefreshFontAssets();

	void AddNewFont(UFont* NewFont);
	UFont* FindFontAssetFromFamily(const FString& FamilyName) const;

	bool HasGoogleFontsInfo() const{ return !GoogleFontsInfo.IsEmpty(); }
	TArray<FGFontFamilyInfo>& GetGoogleFontsInfo() { return GoogleFontsInfo; }
	FGFontFamilyInfo* FindGoogleFontsInfo(const FString& FamilyName);

	static void TryRenameWidget(const FString& InName, TObjectPtr<UWidget> Widget);

	/** Gets a unique widget name using the provided local name tracker */
	static FString GetUniqueWidgetName(TMap<FString, int32>& NameTracker, const FString& BaseName);

	template<class Type>
	static Type* NewWidget(TObjectPtr<UWidgetTree> TreeViewOuter, const FString& NodeName, const FString& WidgetName, TMap<FString, int32>& NameTracker);

	template<class Type>
	static Type* NewWidget(TObjectPtr<UWidgetTree> TreeViewOuter, const FString& NodeName, const FString& WidgetName, UClass* ClassOverride, TMap<FString, int32>& NameTracker);

	UMaterial* GetBorderMaterial() const { return BorderMaterial; }
	void SetBorderMaterial(UMaterial* InBorderMaterial) { BorderMaterial = InBorderMaterial; }

	UMaterialInstanceConstant* GetBorderMaterialInstances(float StrokeWeight) const;
	void AddBorderMaterialInstances(float StrokeWeight, UMaterialInstanceConstant* MaterialInstanceConstant);

	void ResetBorderMaterials();
private:
	UPROPERTY()
	TArray<UFigmaImporter*> Requests;

	UPROPERTY()
	UObjectLibrary* FontObjectLibrary = nullptr;

	UPROPERTY()
	TArray<UFont*> NewFonts;

	UPROPERTY()
	TArray<FGFontFamilyInfo> GoogleFontsInfo;

	UPROPERTY()
	UMaterial* BorderMaterial = nullptr;

	UPROPERTY()
	TMap<float, UMaterialInstanceConstant*> BorderMaterialInstances;

	FFrameToButtonOverride* FrameToButtonOverride = nullptr;
	FClassOverrides* WidgetOverrides = nullptr;

	/** Tracks if an import is currently in progress */
	bool bImportInProgress = false;

	/** Result of the last import operation */
	bool bLastImportSucceeded = false;

	/** Message from the last import (error details if failed) */
	FString LastImportMessage;

	/** Whether the last import failed due to rate limiting */
	bool bLastImportRateLimited = false;

	/** Seconds to wait before retrying (from Retry-After header) */
	int32 RetryAfterSeconds = 60;
};

#define FIND_OVERRIDE_FOR_TYPE(TypeName)																		\
if (Type::StaticClass() == U##TypeName::StaticClass())															\
{																												\
	for (const F##TypeName##Override& ClassOverride : WidgetOverrides->TypeName##Rules)							\
	{																											\
		if (ClassOverride.Match(NodeName) && ClassOverride.ClassOverride != nullptr)							\
			return ClassOverride.ClassOverride;																	\
	}																											\
}


template <class Type>
UClass* UFigmaImportSubsystem::GetOverrideClassForNode(const FString& NodeName)
{
	if (!WidgetOverrides)
		return nullptr;

	FIND_OVERRIDE_FOR_TYPE(Border)
	FIND_OVERRIDE_FOR_TYPE(Button)
	FIND_OVERRIDE_FOR_TYPE(CanvasPanel)
	FIND_OVERRIDE_FOR_TYPE(Image)
	FIND_OVERRIDE_FOR_TYPE(HorizontalBox)
	FIND_OVERRIDE_FOR_TYPE(VerticalBox)
	FIND_OVERRIDE_FOR_TYPE(SizeBox)
	FIND_OVERRIDE_FOR_TYPE(TextBlock)
	FIND_OVERRIDE_FOR_TYPE(WidgetSwitcher)
	FIND_OVERRIDE_FOR_TYPE(WrapBox)

	return nullptr;
}

template <class Type>
Type* UFigmaImportSubsystem::NewWidget(TObjectPtr<UWidgetTree> TreeViewOuter, const FString& NodeName, const FString& WidgetName, TMap<FString, int32>& NameTracker)
{
	UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
	UClass* ClassOverride = Importer ? Importer->GetOverrideClassForNode<Type>(NodeName) : nullptr;
	UClass* WidgetClass = ClassOverride ? ClassOverride : Type::StaticClass();

	// Get unique name using local session tracking
	const FString FinalName = GetUniqueWidgetName(NameTracker, WidgetName);

	// Check if an object with this name already exists (orphan from previous import)
	if (UObject* ExistingObject = StaticFindObject(UObject::StaticClass(), TreeViewOuter, *FinalName))
	{
		// Move orphan to transient to free up the name
		ExistingObject->Rename(nullptr, GetTransientPackage(), REN_DontCreateRedirectors | REN_ForceNoResetLoaders);
		ExistingObject->MarkAsGarbage();
	}

	// Create widget with the tracked name
	return NewObject<Type>(TreeViewOuter, WidgetClass, *FinalName, RF_Transactional);
}

template <class Type>
Type* UFigmaImportSubsystem::NewWidget(TObjectPtr<UWidgetTree> TreeViewOuter, const FString& NodeName, const FString& WidgetName, UClass* ClassOverride, TMap<FString, int32>& NameTracker)
{
	if (!ClassOverride)
	{
		return NewWidget<Type>(TreeViewOuter, NodeName, WidgetName, NameTracker);
	}

	// Get unique name using local session tracking
	const FString FinalName = GetUniqueWidgetName(NameTracker, WidgetName);

	// Check if an object with this name already exists (orphan from previous import)
	if (UObject* ExistingObject = StaticFindObject(UObject::StaticClass(), TreeViewOuter, *FinalName))
	{
		// Move orphan to transient to free up the name
		ExistingObject->Rename(nullptr, GetTransientPackage(), REN_DontCreateRedirectors | REN_ForceNoResetLoaders);
		ExistingObject->MarkAsGarbage();
	}

	// Create widget with the tracked name
	return NewObject<Type>(TreeViewOuter, ClassOverride, *FinalName, RF_Transactional);
}
