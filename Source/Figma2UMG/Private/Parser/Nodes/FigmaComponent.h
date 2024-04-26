// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FigmaFrame.h"
#include "Interfaces/AssetFileHandler.h"
#include "Interfaces/FigmaRefHandle.h"
#include "Parser/Properties/FigmaComponentPropertyDefinition.h"

#include "FigmaComponent.generated.h"

struct FEdGraphPinType;

UCLASS()
class UFigmaComponent : public  UFigmaFrame, public IFigmaRefHandle
{
public:
	GENERATED_BODY()

	// UFigmaNode
	virtual void PrePatchWidget() override;
	virtual TObjectPtr<UWidget> PatchPreInsertWidget(TObjectPtr<UWidget> WidgetToPatch) override;
	virtual void SetWidget(TObjectPtr<UWidget> Widget) override;
	virtual void PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj) override;
	virtual void Reset() override;

	// IFigmaFileHandle
	virtual FString GetPackagePath() const override;
	virtual void LoadOrCreateAssets(UFigmaFile* FigmaFile) override;
	virtual void LoadAssets() override;

	// IWidgetOwner
	virtual void PostInsert() const override;
	virtual void PatchBinds(TObjectPtr<UWidgetBlueprint> WidgetBp) const override;


	bool PatchPropertiesToWidget(UWidgetBlueprint* Widget) const;
	void PatchBinds();

	UWidget* CreateInstance(UObject* AssetOuter) const;

	void TryAddComponentPropertyDefinition(FString PropertyId, FFigmaComponentPropertyDefinition Definition);
protected:
	void FillType(const FFigmaComponentPropertyDefinition& Def, FEdGraphPinType& MemberType) const;

	UPROPERTY()
	TMap<FString, FFigmaComponentPropertyDefinition> ComponentPropertyDefinitions;
};
