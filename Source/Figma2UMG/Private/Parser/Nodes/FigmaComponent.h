// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FigmaFrame.h"
#include "Interfaces/AssetFileHandler.h"
#include "Interfaces/FigmaRefHandle.h"
#include "Parser/Properties/FigmaComponentPropertyDefinition.h"

#include "FigmaComponent.generated.h"

struct FEdGraphPinType;

UCLASS()
class UFigmaComponent : public  UFigmaFrame
{
public:
	GENERATED_BODY()

	// UFigmaNode
	virtual void PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj) override;
	virtual void Reset() override;
	virtual FString GetPackageName() const override;

	// IFigmaFileHandle
	virtual FString GetPackagePath() const override;
	virtual void LoadOrCreateAssets() override;
	virtual void LoadAssets() override;

	// IWidgetOwner
	virtual void PatchBinds(TObjectPtr<UWidgetBlueprint> WidgetBp) const override;

	bool PatchPropertiesToWidget(UWidgetBlueprint* Widget) const;

	void TryAddComponentPropertyDefinition(FString PropertyId, FFigmaComponentPropertyDefinition Definition);
protected:
	void FillType(const FFigmaComponentPropertyDefinition& Def, FEdGraphPinType& MemberType) const;

	UPROPERTY()
	TMap<FString, FFigmaComponentPropertyDefinition> ComponentPropertyDefinitions;
};
