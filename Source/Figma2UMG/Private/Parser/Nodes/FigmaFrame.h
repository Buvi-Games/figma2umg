// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FigmaGroup.h"
#include "Interfaces/AssetFileHandler.h"
#include "Interfaces/FigmaRefHandle.h"
#include "Parser/Properties/FigmaLayoutGrid.h"

#include "FigmaFrame.generated.h"

class UWidgetBlueprintBuilder;

UCLASS()
class UFigmaFrame : public  UFigmaGroup, public IFigmaFileHandle, public IFigmaRefHandle
{
public:
	GENERATED_BODY()

	void SetGenerateFile(bool Value = true);

	// UFigmaNode
	virtual TObjectPtr<UWidget> PatchPreInsertWidget(TObjectPtr<UWidget> WidgetToPatch) override;
	virtual TScriptInterface<IWidgetBuilder> CreateWidgetBuilders(bool IsRoot = false) const override;
	virtual void SetWidget(TObjectPtr<UWidget> Widget) override;
	virtual TScriptInterface<IAssetBuilder> CreateAssetBuilder(const FString& InFileKey) override;
	virtual FString GetPackageName() const override;

	// IFigmaFileHandle
	virtual FString GetPackagePath() const override;
	virtual FString GetAssetName() const override;
	virtual void LoadOrCreateAssets() override;
	virtual void LoadAssets() override;

	// IWidgetOwner
	virtual void PostInsert() const override;

	void PatchBluePrintBinds() const;
	virtual UObject* GetAssetOuter() const override;

	UWidget* CreateInstance(UObject* InAssetOuter) const;

	const TObjectPtr<UWidgetBlueprintBuilder>& GetAssetBuilder() const;
protected:

	UPROPERTY()
	TArray<FFigmaLayoutGrid> LayoutGrids;

	UPROPERTY()
	TObjectPtr<UWidgetBlueprintBuilder> WidgetBlueprintBuilder = nullptr;

	bool GenerateFile = false;
};
