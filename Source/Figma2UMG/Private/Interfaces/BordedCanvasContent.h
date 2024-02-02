// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"

#include "BordedCanvasContent.generated.h"

UINTERFACE(BlueprintType, Experimental, meta = (CannotImplementInterfaceInBlueprint))
class FIGMA2UMG_API UBordedCanvasContent : public UInterface
{
	GENERATED_BODY()
};

class FIGMA2UMG_API IBordedCanvasContent
{
	GENERATED_BODY()
public:
	UFUNCTION()
	virtual FVector2D GetPosition() const = 0;

	UFUNCTION()
	virtual FVector2D GetSize() const = 0;

	TObjectPtr<UBorder> AddOrPathContent(UBorder* BorderToPatch, UObject* Outer, const FString& UniqueName);
	void PostInsertContent(FLinearColor BrushColor = FLinearColor::White) const;

protected:
	UBorder* Border = nullptr;
	UCanvasPanel* Canvas = nullptr;
};