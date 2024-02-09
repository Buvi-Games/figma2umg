// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Builder/TextBoxBuilder.h"
#include "Interfaces/WidgetOwner.h"
#include "Parser/Nodes/Vectors/FigmaVectorNode.h"
#include "Parser/Properties/FigmaTypeStyle.h"

#include "FigmaText.generated.h"

class UTextBlock;

UENUM()
enum class EFigmaLineType
{
	ORDERED,// : Text is an ordered list (numbered)
	UNORDERED,// : Text is an unordered list (bulleted)
	NONE,// : Text is plain text and not part of any list
};

UCLASS()
class UFigmaText : public UFigmaVectorNode, public IWidgetOwner
{
public:
	GENERATED_BODY()

	// IWidgetOwner
	virtual void ForEach(const FOnEachFunction& Function) override;
	virtual TObjectPtr<UWidget> Patch(TObjectPtr<UWidget> WidgetToPatch) override;
	virtual TObjectPtr<UWidget> GetTopWidget() const override;
	virtual FVector2D GetTopWidgetPosition() const override;
	virtual TObjectPtr<UPanelWidget> GetContainerWidget() const override;

	// IFigmaImageRequester
	virtual void AddImageRequest(FImageRequests& ImageRequests) override {};

protected:

	UPROPERTY()
	FString Characters;

	UPROPERTY()
	FFigmaTypeStyle Style;

	UPROPERTY()
	TArray<int> CharacterStyleOverrides;

	UPROPERTY()
	TMap<int, FFigmaTypeStyle> StyleOverrideTable;

	UPROPERTY()
	TArray<EFigmaLineType> LineTypes;

	UPROPERTY()
	TArray<int> LineIndentations;

	UPROPERTY()
	FTextBoxBuilder Builder;
};
