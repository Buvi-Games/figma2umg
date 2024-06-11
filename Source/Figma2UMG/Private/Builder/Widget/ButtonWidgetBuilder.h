// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SingleChildBuilder.h"
#include "Parser/Nodes/FigmaComponent.h"
#include "ButtonWidgetBuilder.generated.h"

class UButton;

UCLASS()
class UButtonWidgetBuilder : public USingleChildBuilder
{
public:
	GENERATED_BODY()
	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) override;

	void SetDefaultNode(const UFigmaGroup* InNode);
	void SetHoveredNode(const UFigmaGroup* InNode);
	void SetPressedNode(const UFigmaGroup* InNode);
	void SetDisabledNode(const UFigmaGroup* InNode);
	void SetFocusedNode(const UFigmaGroup* InNode);

	virtual void SetWidget(const TObjectPtr<UWidget>& InWidget) override;
	virtual void ResetWidget() override;
protected:
	virtual TObjectPtr<UContentWidget> GetContentWidget() const override;
	virtual void GetPaddingValue(FMargin& Padding) const override;

	void Setup() const;
	void SetupBrush(FSlateBrush& Brush, const UFigmaGroup& FigmaGroup) const;

	UPROPERTY()
	TObjectPtr<UButton> Widget = nullptr;

	UPROPERTY()
	const UFigmaGroup* DefaultNode = nullptr;

	UPROPERTY()
	const UFigmaGroup* HoveredNode = nullptr;

	UPROPERTY()
	const UFigmaGroup* PressedNode = nullptr;

	UPROPERTY()
	const UFigmaGroup* DisabledNode = nullptr;

	UPROPERTY()
	const UFigmaGroup* FocusedNode = nullptr;
};
