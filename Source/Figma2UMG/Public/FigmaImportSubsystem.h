// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "REST/FigmaImporter.h"

#include "FigmaImportSubsystem.generated.h"

UCLASS()
class FIGMA2UMG_API UFigmaImportSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()
public:
	UFigmaImporter* Request(const FString& InAccessToken, const FString& InFileKey, const FString& InIds, const FString& InContentRootFolder, const FOnFigmaImportUpdateStatusCB& InRequesterCallback);
	void RemoveRequest(UFigmaImporter* FigmaImporter);

private:
	UPROPERTY()
	TArray<UFigmaImporter*> Requests;
};
