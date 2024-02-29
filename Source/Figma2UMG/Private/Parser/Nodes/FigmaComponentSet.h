// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FigmaFrame.h"
#include "Builder/SwitcherBuilder.h"
#include "Interfaces/AssetFileHandler.h"
#include "Parser/Properties/FigmaComponentPropertyDefinition.h"

#include "FigmaComponentSet.generated.h"

UCLASS()
class UFigmaComponentSet : public  UFigmaFrame, public IFigmaFileHandle
{
public:
	GENERATED_BODY()

	// UFigmaNode
	virtual void PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj) override;
	virtual void PrePatchWidget() override;
	virtual TObjectPtr<UWidget> PatchPreInsertWidget(TObjectPtr<UWidget> WidgetToPatch) override;

	// IFigmaFileHandle
	virtual FString GetPackagePath() const override;
	virtual FString GetAssetName() const override;
	virtual void LoadOrCreateAssets(UFigmaFile* FigmaFile) override;

	// IWidgetOwner
	virtual TObjectPtr<UWidget> Patch(TObjectPtr<UWidget> WidgetToPatch) override;
	virtual void PostInsert() const override;
	virtual void PatchBinds(TObjectPtr<UWidgetBlueprint> WidgetBp) const override;

	// IFigmaContainer
	virtual TArray<UFigmaNode*>& GetChildren() override;
	void FillType(const FFigmaComponentPropertyDefinition& Def, FEdGraphPinType& MemberType) const;


protected:
	bool PatchPropertiesToWidget(UWidgetBlueprint* Widget);
	void PatchBinds();

	UPROPERTY()
	TMap<FString, FFigmaComponentPropertyDefinition> ComponentPropertyDefinitions;

	FSwitcherBuilder Builder;

	bool IsButton = false;
	TArray<UFigmaNode*> Empty;
};
