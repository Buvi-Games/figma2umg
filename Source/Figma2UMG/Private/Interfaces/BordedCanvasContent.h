// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetOwner.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"

#include "BordedCanvasContent.generated.h"

UINTERFACE(BlueprintType, Experimental, meta = (CannotImplementInterfaceInBlueprint))
class FIGMA2UMG_API UBordedCanvasContent : public UInterface
{
	GENERATED_BODY()
};

class FIGMA2UMG_API IBordedCanvasContent : public IWidgetOwner
{
	GENERATED_BODY()
public:
	UFUNCTION()
	virtual FVector2D GetSize() const = 0;

	UFUNCTION()
	virtual FLinearColor GetBrushColor() const = 0;

	TObjectPtr<UBorder> AddOrPatchContent(UBorder* BorderToPatch, UObject* Outer, const FString& UniqueName);

	// IWidgetOwner
	virtual void ForEach(const FOnEachFunction& Function) override;
	virtual void PostInsert() const override;
	virtual TObjectPtr<UWidget> GetTopWidget() const override;
	virtual TObjectPtr<UPanelWidget> GetContainerWidget() const override;
protected:
	UBorder* Border = nullptr;
	UCanvasPanel* Canvas = nullptr;
};