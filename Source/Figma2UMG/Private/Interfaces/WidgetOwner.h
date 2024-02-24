// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "WidgetOwner.generated.h"

class UWidgetBlueprint;
class UPanelWidget;
class UWidget;
class UFigmaNode;

UINTERFACE(BlueprintType, Experimental, meta = (CannotImplementInterfaceInBlueprint))
class FIGMA2UMG_API UWidgetOwner : public UInterface
{
	GENERATED_BODY()
};

class FIGMA2UMG_API IWidgetOwner
{
	GENERATED_BODY()
public:
	DECLARE_DELEGATE_OneParam(FOnEachFunction, UWidget&)
	virtual void ForEach(const FOnEachFunction& Function) = 0;

	virtual TObjectPtr<UWidget> Patch(TObjectPtr<UWidget> WidgetToPatch) = 0;
	virtual void PostInsert() const;
	virtual void Reset() = 0;

	virtual TObjectPtr<UWidget> GetTopWidget() const = 0;
	virtual FVector2D GetTopWidgetPosition() const = 0;

	virtual TObjectPtr<UPanelWidget> GetContainerWidget() const = 0;

	virtual void PatchBinds(TObjectPtr<UWidgetBlueprint> WidgetBp) const = 0;

	void SetPosition(TObjectPtr<UWidget> Widget, const FVector2D& Size) const;
	void SetSize(TObjectPtr<UWidget> Widget, const FVector2D& Size) const;
};
