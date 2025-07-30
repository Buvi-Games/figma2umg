// MIT License
// Copyright (c) 2024 Buvi Games

#pragma once

#include "CoreMinimal.h"
#include "FigmaGroup.h"
#include "Parser/Properties/FigmaLayoutGrid.h"

#include "FigmaFrame.generated.h"

class UWidgetBlueprintBuilder;

UCLASS()
class FIGMA2UMG_API UFigmaFrame : public  UFigmaGroup
{
public:
	GENERATED_BODY()

	void SetGenerateFile(bool Value = true);

	// UFigmaNode
	virtual TScriptInterface<IWidgetBuilder> CreateWidgetBuilders(bool IsRoot = false, bool AllowFrameButton = true) const override;
	virtual bool CreateAssetBuilder(const FString& InFileKey, TArray<TScriptInterface<IAssetBuilder>>& AssetBuilders) override;
	virtual FString GetPackageNameForBuilder(const TScriptInterface<IAssetBuilder>& InAssetBuilder) const override;

	const TObjectPtr<UWidgetBlueprintBuilder>& GetAssetBuilder() const;
protected:

	UPROPERTY()
	TArray<FFigmaLayoutGrid> LayoutGrids;

	UPROPERTY()
	TObjectPtr<UWidgetBlueprintBuilder> WidgetBlueprintBuilder = nullptr;

	bool GenerateFile = false;
};
