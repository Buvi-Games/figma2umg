// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FigmaGroup.h"
#include "Parser/Properties/FigmaLayoutGrid.h"

#include "FigmaFrame.generated.h"

class UWidgetBlueprintBuilder;

UCLASS()
class UFigmaFrame : public  UFigmaGroup
{
public:
	GENERATED_BODY()

	void SetGenerateFile(bool Value = true);

	// UFigmaNode
	virtual TScriptInterface<IWidgetBuilder> CreateWidgetBuilders(bool IsRoot = false, bool AllowFrameButton = true) const override;
	virtual TScriptInterface<IAssetBuilder> CreateAssetBuilder(const FString& InFileKey) override;
	virtual FString GetPackageName() const override;

	const TObjectPtr<UWidgetBlueprintBuilder>& GetAssetBuilder() const;
protected:

	UPROPERTY()
	TArray<FFigmaLayoutGrid> LayoutGrids;

	UPROPERTY()
	TObjectPtr<UWidgetBlueprintBuilder> WidgetBlueprintBuilder = nullptr;

	bool GenerateFile = false;
};
