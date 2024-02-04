// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FigmaNode.generated.h"

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

	virtual void PostInsert(UWidget* Widget) const;

	virtual void PrePatchWidget();
	TObjectPtr<UWidget> PatchWidget(TObjectPtr<UWidget> WidgetToPatch);
	void PostPatchWidget();

	FString GetId() const { return Id; }

	FString GetNodeName() const;
	FString GetUniqueName() const;
	ESlateVisibility GetVisibility() const;

	FVector2D GetPosition() const;

	virtual FVector2D GetAbsolutePosition() const PURE_VIRTUAL(UFigmaNode::GetAbsolutePosition(), return FVector2D();)

	void SetCurrentPackagePath(const FString & InPackagePath);
	virtual FString GetCurrentPackagePath() const;

	virtual TObjectPtr<UFigmaFile> GetFigmaFile() const;

	UObject* GetAssetOuter() const;
	TObjectPtr<UFigmaNode> GetParentNode() const { return ParentNode; }

protected:
	virtual TObjectPtr<UWidget> PatchWidgetImp(TObjectPtr<UWidget> WidgetToPatch) { return nullptr; }

	void SerializeArray(TArray<UFigmaNode*>& Array, const TSharedRef<FJsonObject> JsonObj, const FString& arrayName);

	UFigmaNode* CreateNode(const TSharedPtr<FJsonObject>& JsonObj);

	void AddOrPathChildren(UPanelWidget* ParentWidget, TArray<UFigmaNode*> Children) const;;

	TObjectPtr<UFigmaNode> ParentNode = nullptr;

	FString PackagePath;
private:
	UPROPERTY()
	FString Id;

	UPROPERTY()
	FString Name;

	UPROPERTY()
	bool Visible;

	UPROPERTY()
	FString Type;

	UPROPERTY()
	float Rotation;

	UPROPERTY()
	FString PluginData;

	UPROPERTY()
	FString SharedPluginData;

	UPROPERTY()
	TMap<FString, FString> ComponentPropertyReferences;

	//boundVariablesMap - beta
	//explicitVariableModesMap - beta

	UPROPERTY()
	FString ScrollBehaviour; //Not in doc
};
