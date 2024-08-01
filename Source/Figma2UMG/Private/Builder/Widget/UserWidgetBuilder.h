// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_ComponentBoundEvent.h"
#include "WidgetBuilder.h"
#include "UserWidgetBuilder.generated.h"

class IFlowTransition;
class UWidgetBlueprintBuilder;
class UTexture2DBuilder;
class UWidget;
class UImage;

UCLASS()
class UUserWidgetBuilder : public UObject, public IWidgetBuilder
{
public:
	GENERATED_BODY()
	void SetWidgetBlueprintBuilder(const TObjectPtr<UWidgetBlueprintBuilder>& InWidgetBlueprintBuilder);

	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UWidget>& WidgetToPatch) override;
	virtual bool TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget) override;
	virtual void PatchWidgetProperties() override;

	virtual void SetWidget(const TObjectPtr<UWidget>& InWidget) override;
	virtual TObjectPtr<UWidget> GetWidget() const override;
	virtual void ResetWidget() override;
protected:
	virtual void GetPaddingValue(FMargin& Padding) const override;
	virtual bool GetAlignmentValues(EHorizontalAlignment& HorizontalAlignment, EVerticalAlignment& VerticalAlignment) const override;
	void SetupTransition(const IFlowTransition* FlowTransition) const;

	UK2Node_CallFunction* AddFunctionAfterNode(const TObjectPtr<UWidgetBlueprint>& WidgetBlueprint, const UEdGraphNode* PreviousNode, const FString& FunctionName) const;
	UEdGraphNode* AddNodeAfterNode(const UK2Node* PreviousNode, TSubclassOf<UEdGraphNode> const NodeClass) const;

	UPROPERTY()
	TObjectPtr<UWidgetBlueprintBuilder> WidgetBlueprintBuilder = nullptr;

	UPROPERTY()
	TObjectPtr<UUserWidget> Widget = nullptr;
};
