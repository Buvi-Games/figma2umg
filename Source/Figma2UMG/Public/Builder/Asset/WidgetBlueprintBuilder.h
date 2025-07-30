// MIT License
// Copyright (c) 2024 Buvi Games

#pragma once

#include "CoreMinimal.h"
#include "AssetBuilder.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "WidgetBlueprintBuilder.generated.h"

class IWidgetBuilder;
struct FFigmaComponentPropertyDefinition;
class UWidgetBlueprint;

UCLASS()
class FIGMA2UMG_API UWidgetBlueprintBuilder : public UObject, public IAssetBuilder
{
	GENERATED_BODY()
public:
	virtual void LoadOrCreateAssets() override;
	virtual void LoadAssets() override;
	virtual void Reset() override;

	virtual void ResetWidgets();

	void CompileBP(EBlueprintCompileOptions CompileFlags);

	void CreateWidgetBuilders();
	void PatchAndInsertWidgets();
	void PatchWidgetBinds();
	void PatchWidgetProperties();

	TObjectPtr<UWidgetBlueprint> GetAsset() const;

	virtual UPackage* GetAssetPackage() const override;
protected:
	void FillType(const FFigmaComponentPropertyDefinition& Def, FEdGraphPinType& MemberType) const;
	void PatchMemberVariable(UWidgetBlueprint* WidgetBP, TPair<FString, FFigmaComponentPropertyDefinition> Property) const;
	void PatchPropertyDefinitions(const TMap<FString, FFigmaComponentPropertyDefinition>& ComponentPropertyDefinitions) const;

	UPROPERTY()
	TObjectPtr<UWidgetBlueprint> Asset = nullptr;

	UPROPERTY()
	TScriptInterface<IWidgetBuilder> RootWidgetBuilder = nullptr;
};
