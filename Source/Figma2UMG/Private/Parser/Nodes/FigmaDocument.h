// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/AssetFileHandler.h"
#include "Interfaces/FigmaContainer.h"
#include "Parser/Nodes/FigmaNode.h"

#include "FigmaDocument.generated.h"

class UWidgetBlueprint;
class UFigmaFile;

UCLASS()
class UFigmaDocument : public UFigmaNode, public IFigmaFileHandle, public IFigmaContainer
{
public:
	GENERATED_BODY()

	// UFigmaNode
	virtual FVector2D GetAbsolutePosition() const override { return FVector2D::ZeroVector; }
	virtual TObjectPtr<UFigmaFile> GetFigmaFile() const override { return FigmaFile; }
	virtual void PrePatchWidget() override;
	virtual TObjectPtr<UWidget> PatchPreInsertWidget(TObjectPtr<UWidget> WidgetToPatch) override;
	virtual void SetWidget(TObjectPtr<UWidget> Widget) override;
	virtual TScriptInterface<IAssetBuilder> CreateAssetBuilder(const FString& InFileKey) override;
	virtual FString GetPackageName() const override;
	virtual FString GetUAssetName() const override;

	// IFigmaFileHandle
	virtual FString GetPackagePath() const override;
	virtual FString GetAssetName() const override;
	virtual void LoadOrCreateAssets() override;
	virtual void LoadAssets() override;

	// IFigmaContainer
	virtual FString GetJsonArrayName() const override { return FString("Children"); }
	virtual TArray<UFigmaNode*>& GetChildren() override { return Children; }

	void SetFigmaFile(UFigmaFile* InFigmaFile);

protected:

	TObjectPtr<UFigmaFile> FigmaFile = nullptr;

	UPROPERTY()
	TArray<UFigmaNode*> Children;
};
