// Fvirtual ill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FigmaNode.generated.h"

class UK2Node_FunctionResult;
class UK2Node_VariableGet;
class UK2Node_IfThenElse;
class UWidgetBlueprint;
class UFigmaFile;
class UWidgetTree;
class UPanelWidget;
class UWidget;

UENUM()
enum class ENodeTypes
{
	DOCUMENT,
	CANVAS,
	FRAME,
	GROUP,
	SECTION,
	VECTOR,
	BOOLEAN_OPERATION,
	STAR,
	LINE,
	ELLIPSE,
	REGULAR_POLYGON,
	RECTANGLE,
	TABLE,
	TABLE_CELL,
	TEXT,
	SLICE,
	COMPONENT,
	COMPONENT_SET,
	INSTANCE,
	STICKY,
	SHAPE_WITH_TEXT,
	CONNECTOR,
	WASHI_TAPE,
};

UCLASS()
class UFigmaNode : public UObject
{
public:
	GENERATED_BODY()

	virtual void PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj);

	//virtual void PostInsert(UWidget* Widget) const;

	virtual void PrePatchWidget();
	virtual TObjectPtr<UWidget> PatchPreInsertWidget(TObjectPtr<UWidget> WidgetToPatch);
	virtual void PatchPostInsertWidget();
	void PostPatchWidget();

	FString GetId() const { return Id; }

	FString GetNodeName() const;
	FString GetUniqueName() const;
	ESlateVisibility GetVisibility() const;

	FVector2D GetPosition() const;

	virtual FVector2D GetAbsolutePosition() const PURE_VIRTUAL(UFigmaNode::GetAbsolutePosition(), return FVector2D::ZeroVector;)

	void SetCurrentPackagePath(const FString & InPackagePath);
	virtual FString GetCurrentPackagePath() const;

	virtual TObjectPtr<UFigmaFile> GetFigmaFile() const;

	UObject* GetAssetOuter() const;
	TObjectPtr<UFigmaNode> GetParentNode() const { return ParentNode; }
	TObjectPtr<UFigmaNode> FindTypeByID(const UClass* Class, const FString& ID);

	const TMap<FString, FString>& GetComponentPropertyReferences() const { return ComponentPropertyReferences; }
protected:
	void SerializeArray(TArray<UFigmaNode*>& Array, const TSharedRef<FJsonObject> JsonObj, const FString& arrayName);

	UFigmaNode* CreateNode(const TSharedPtr<FJsonObject>& JsonObj);
	void ProcessComponentPropertyReferences(TObjectPtr<UWidgetBlueprint> WidgetBP, TObjectPtr<UWidget> Widget) const;

//	void PatchPreInsertWidgetChildren(UPanelWidget* ParentWidget, const TArray<UFigmaNode*>& Children) const;;

	TObjectPtr<UFigmaNode> ParentNode = nullptr;

	FString PackagePath;
private:
	virtual void ProcessComponentPropertyReference(TObjectPtr<UWidgetBlueprint> WidgetBP, TObjectPtr<UWidget> Widget, const TPair<FString, FString>& PropertyReference) const;

	void AddBinding(TObjectPtr<UWidgetBlueprint> WidgetBP, TObjectPtr<UWidget> Widget, UEdGraph* FunctionGraph, const FName& PropertyName) const;

	void PatchVisibilityBind(TObjectPtr<UWidgetBlueprint> WidgetBP, TObjectPtr<UWidget> Widget, const FBPVariableDescription& VariableDescription, const FName& VariableName) const;

	UK2Node_VariableGet* PatchVariableGetNode(TObjectPtr<UWidgetBlueprint> WidgetBP, UEdGraph* Graph, FName VariableName, FVector2D NodeLocation) const;
	UK2Node_IfThenElse* PatchIfThenElseNode(UEdGraph* Graph, FVector2D NodeLocation, UEdGraphPin* ExecPin, UEdGraphPin* ConditionValuePin, UEdGraphPin* ThenReturnPin, UEdGraphPin* ElseReturnPin) const;
	UK2Node_FunctionResult* PatchFunctionResult(UEdGraph* Graph, FVector2D NodeLocation, const FString& ReturnValue) const;

	UPROPERTY()
	FString Id;

	UPROPERTY()
	FString Name;

	UPROPERTY()
	bool Visible = true;

	UPROPERTY()
	ENodeTypes Type;

	UPROPERTY()
	float Rotation = 0.0f;

	UPROPERTY()
	FString PluginData;

	UPROPERTY()
	FString SharedPluginData;

protected:
	UPROPERTY()
	TMap<FString, FString> ComponentPropertyReferences;

	//boundVariablesMap - beta
	//explicitVariableModesMap - beta

	UPROPERTY()
	FString ScrollBehaviour; //Not in doc
};
