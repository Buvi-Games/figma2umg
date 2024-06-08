// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetBuilder.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "WidgetBlueprintBuilder.generated.h"

class IWidgetBuilder;
struct FFigmaComponentPropertyDefinition;
class UWidgetBlueprint;

UCLASS()
class UWidgetBlueprintBuilder : public UObject, public IAssetBuilder
{
	GENERATED_BODY()
public:
	virtual void LoadOrCreateAssets() override;
	virtual void LoadAssets() override;
	virtual void ResetWidgets();

	void CompileBP(EBlueprintCompileOptions CompileFlags);

	void CreateWidgetBuilders();
	void PatchAndInsertWidgets();
	void PatchWidgetBinds();
	void PatchWidgetProperties();

	TObjectPtr<UWidgetBlueprint> GetAsset() const;
protected:
	void FillType(const FFigmaComponentPropertyDefinition& Def, FEdGraphPinType& MemberType) const;
	bool PatchPropertyDefinitions(const TMap<FString, FFigmaComponentPropertyDefinition>& ComponentPropertyDefinitions) const;

	UPROPERTY()
	TObjectPtr<UWidgetBlueprint> Asset = nullptr;

	UPROPERTY()
	TScriptInterface<IWidgetBuilder> RootWidgetBuilder = nullptr;
};
