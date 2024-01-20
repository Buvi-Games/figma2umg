// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FigmaFile.generated.h"

USTRUCT()
struct FFigmaFile
{
public:
	GENERATED_BODY()

	void PostSerialize(const TSharedRef<FJsonObject> jsonObj);

private:

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

	// UPROPERTY()
	//"document": {}, //TODO: Not sure what is inside

	// UPROPERTY()
	//"components": {}, //TODO: Not sure what is inside

	// UPROPERTY()
	//"componentSets": {}, //TODO: Not sure what is inside

	// UPROPERTY()
	//"styles": {}, //TODO: Not sure what is inside
};
