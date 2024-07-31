// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "Rest/Enums.h"
#include "Settings/ClassOverrides.h"
#include "REST/Gfonts/GFontRequest.h"

#include "FigmaImportSubsystem.generated.h"

class UObjectLibrary;
struct FClassOverrides;
class UFigmaImporter;
class URequestParams;

DECLARE_DELEGATE_TwoParams(FOnFigmaImportUpdateStatusCB, eRequestStatus, FString);

UCLASS()
class FIGMA2UMG_API UFigmaImportSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()
public:
	UFigmaImporter* Request(const TObjectPtr<URequestParams> InProperties, const FOnFigmaImportUpdateStatusCB& InRequesterCallback);
	void RemoveRequest(UFigmaImporter* FigmaImporter);

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

	template<class Type>
	static Type* NewWidget(UObject* TreeViewOuter, const FString& NodeName, const FString& WidgetName);

	template<class Type>
	static Type* NewWidget(UObject* TreeViewOuter, const FString& NodeName, const FString& WidgetName, UClass* ClassOverride);

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
Type* UFigmaImportSubsystem::NewWidget(UObject* TreeViewOuter, const FString& NodeName, const FString& WidgetName)
{
	const FString UniqueName = MakeUniqueObjectName(TreeViewOuter, Type::StaticClass(), *WidgetName).ToString();
	UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
	UClass* ClassOverride = Importer ? Importer->GetOverrideClassForNode<Type>(NodeName) : nullptr;
	if (ClassOverride)
	{
		return NewObject<Type>(TreeViewOuter, ClassOverride, *UniqueName);
	}
	else
	{
		return NewObject<Type>(TreeViewOuter, *UniqueName);
	}
}

template <class Type>
Type* UFigmaImportSubsystem::NewWidget(UObject* TreeViewOuter, const FString& NodeName, const FString& WidgetName, UClass* ClassOverride)
{
	if (!ClassOverride)
	{
		return NewWidget<Type>(TreeViewOuter, NodeName, WidgetName);
	}

	const FString UniqueName = MakeUniqueObjectName(TreeViewOuter, ClassOverride, *WidgetName).ToString();
	return NewObject<Type>(TreeViewOuter, ClassOverride, *UniqueName);
}
