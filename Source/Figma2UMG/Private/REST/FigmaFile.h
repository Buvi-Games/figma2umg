// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SImporterWidget.h"

#include "FigmaFile.generated.h"

class UFigmaDocument;
struct FFigmaComponentRef;
struct FFigmaComponentSetRef;
struct FFigmaStyleRef;

UCLASS()
class UFigmaFile : public UObject
{
public:
	GENERATED_BODY()

	void PostSerialize(const TSharedRef<FJsonObject> JsonObj);
	void CreateOrUpdateAsset(const FString& ContentRootFolder);

protected:

	UPROPERTY()
	int SchemaVersion = 0;

	UPROPERTY()
	FString Name;

	UPROPERTY()
	FString LastModified;

	UPROPERTY()
	FString ThumbnailUrl;

	UPROPERTY()
	int Version = 0;

	UPROPERTY()
	FString Role;

	UPROPERTY()
	FString EditorType;

	UPROPERTY()
	FString LinkAccess;

	UPROPERTY()
	UFigmaDocument* Document;

	UPROPERTY()
	TMap<FString, FFigmaComponentRef> Components;

	UPROPERTY()
	TMap<FString, FFigmaComponentSetRef> ComponentSets;

	UPROPERTY()
	TMap<FString, FFigmaStyleRef> Styles;//Not sure if this is correct, probably not
};
