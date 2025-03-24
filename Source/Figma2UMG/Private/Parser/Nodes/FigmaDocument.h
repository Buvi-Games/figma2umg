// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/FigmaContainer.h"
#include "Parser/Nodes/FigmaNode.h"

#include "FigmaDocument.generated.h"

class UWidgetBlueprint;
class UFigmaFile;

UCLASS()
class FIGMA2UMG_API UFigmaDocument : public UFigmaNode, public IFigmaContainer
{
public:
	GENERATED_BODY()

	// UFigmaNode
	virtual FVector2D GetAbsolutePosition(const bool IsTopWidgetForNode) const override { return FVector2D::ZeroVector; }
	virtual TObjectPtr<UFigmaFile> GetFigmaFile() const override { return FigmaFile; }
	virtual bool CreateAssetBuilder(const FString& InFileKey, TArray<TScriptInterface<IAssetBuilder>>& AssetBuilders) override;
	virtual FString GetPackageNameForBuilder(const TScriptInterface<IAssetBuilder>& InAssetBuilder) const override;
	virtual FString GetUAssetName() const override;
	virtual TScriptInterface<IWidgetBuilder> CreateWidgetBuilders(bool IsRoot = false, bool AllowFrameButton = true) const override;

	// IFigmaContainer
	virtual FString GetJsonArrayName() const override { return FString("Children"); }
	virtual TArray<UFigmaNode*>& GetChildren() override { return Children; }
	virtual const TArray<UFigmaNode*>& GetChildrenConst() const override { return Children; }

	void SetFigmaFile(UFigmaFile* InFigmaFile);

protected:

	TObjectPtr<UFigmaFile> FigmaFile = nullptr;

	UPROPERTY()
	TArray<UFigmaNode*> Children;
};
