// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBuilder.h"
#include "UserWidgetBuilder.generated.h"

class UFigmaInstance;
class IFlowTransition;
class UWidgetBlueprintBuilder;
class UTexture2DBuilder;
class UWidget;
class UImage;

UCLASS()
class FIGMA2UMG_API UUserWidgetBuilder : public UObject, public IWidgetBuilder
{
public:
	GENERATED_BODY()
	void SetWidgetBlueprintBuilder(const TObjectPtr<UWidgetBlueprintBuilder>& InWidgetBlueprintBuilder);

	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UWidget>& WidgetToPatch) override;
	virtual void PostInsertWidgets(TObjectPtr<UWidgetBlueprint> WidgetBlueprint) override;
	virtual bool TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget) override;
	virtual void PatchWidgetBinds(const TObjectPtr<UWidgetBlueprint>& WidgetBlueprint);
	virtual void PatchWidgetProperties() override;

	virtual void SetWidget(const TObjectPtr<UWidget>& InWidget) override;
	virtual TObjectPtr<UWidget> GetWidget() const override;
	virtual void ResetWidget() override;
protected:
	virtual void GetPaddingValue(FMargin& Padding) const override;
	virtual bool GetAlignmentValues(EHorizontalAlignment& HorizontalAlignment, EVerticalAlignment& VerticalAlignment) const override;
	void SetupTransitions(const IFlowTransition* FlowTransition) const;
	void SetupTransition(const IFlowTransition* FlowTransition, TObjectPtr<UWidgetBlueprint> WidgetBlueprint, FName EventName, FObjectProperty* VariableProperty) const;

	UEdGraphNode* AddNodeAfterNode(const UK2Node* PreviousNode, TSubclassOf<UEdGraphNode> const NodeClass) const;
	void SetupEventDispatcher(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const FName& EventName) const;
	void PatchEvents(const TObjectPtr<UWidgetBlueprint>& WidgetBlueprint);
	void PatchEvent(const TObjectPtr<UWidgetBlueprint>& WidgetBlueprint, FObjectProperty* VariableProperty, const FName& EventName, const FName& EventDispatchersName);
	void PatchButtonsEnabled(const TObjectPtr<UWidgetBlueprint>& WidgetBlueprint);
	void PatchRelayEnabledFunction(const TObjectPtr<UWidgetBlueprint>& WidgetBlueprint, const FString& FunctionName);
	void PatchInteractiveStateDisabled(const UFigmaInstance* FigmaInstance);

	UPROPERTY()
	TObjectPtr<UWidgetBlueprintBuilder> WidgetBlueprintBuilder = nullptr;

	UPROPERTY()
	TObjectPtr<UUserWidget> Widget = nullptr;
};
