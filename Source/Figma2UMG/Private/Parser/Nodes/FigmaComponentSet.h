// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FigmaFrame.h"
#include "Builder/ButtonBuilder.h"
#include "Builder/SwitcherBuilder.h"
#include "Interfaces/AssetFileHandler.h"
#include "Parser/Properties/FigmaComponentPropertyDefinition.h"

#include "FigmaComponentSet.generated.h"

UCLASS()
class UFigmaComponentSet : public  UFigmaFrame
{
public:
	GENERATED_BODY()

	// UFigmaNode
	virtual void PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj) override;
	virtual void PrePatchWidget() override;
	virtual TObjectPtr<UWidget> PatchPreInsertWidget(TObjectPtr<UWidget> WidgetToPatch) override;
	virtual void SetWidget(TObjectPtr<UWidget> Widget) override;
	virtual UObject* GetAssetOuter() const override;
	virtual TScriptInterface<IWidgetBuilder> CreateWidgetBuilders(bool IsRoot = false) const override;
	virtual void Reset() override;
	virtual FString GetPackageName() const override;

	// IFigmaFileHandle
	virtual FString GetPackagePath() const override;
	virtual void LoadOrCreateAssets() override;

	// IWidgetOwner
	virtual TObjectPtr<UWidget> GetTopWidget() const override;
	virtual TObjectPtr<UPanelWidget> GetContainerWidget() const override;
	virtual void PostInsert() const override;
	virtual void PatchBinds(TObjectPtr<UWidgetBlueprint> WidgetBp) const override;

	// IFigmaContainer
	virtual TArray<UFigmaNode*>& GetChildren() override;
	void FillType(const FFigmaComponentPropertyDefinition& Def, FEdGraphPinType& MemberType) const;

	UPROPERTY()
	TMap<FString, FFigmaComponentPropertyDefinition> ComponentPropertyDefinitions;

protected:
	TObjectPtr<UWidget> PatchVariation(TObjectPtr<UWidget> WidgetToPatch);
	bool PatchPropertiesToWidget(UWidgetBlueprint* Widget);

	void PatchInitFunction(const TPair< FString, FFigmaComponentPropertyDefinition>& PropertyDefinition) const;

	TObjectPtr<UWidgetSwitcher> FindSwitcher(const FString& SwitcherName) const;

	TArray<FSwitcherBuilder> SwitchBuilders;
	TArray<FButtonBuilder> ButtonBuilders;

	bool IsDoingInPlace = false;
	TArray<UFigmaNode*> ButtonSubNodes;
};
