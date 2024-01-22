// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FigmaNode.generated.h"

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

	virtual void PostSerialize(const TSharedRef<FJsonObject> JsonObj) {};

protected:
	void SerializeArray(TArray<UFigmaNode*>& Array, const TSharedRef<FJsonObject> JsonObj, const FString& arrayName);

	UFigmaNode* CreateNode(const TSharedPtr<FJsonObject>& JsonObj);

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
