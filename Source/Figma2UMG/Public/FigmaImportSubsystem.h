// Copyright 2024 Buvi Games. All Rights Reserved.

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
	UFigmaImporter* Request(const TObjectPtr<URequestParams> InProperties, const FOnFigmaImportUpdateStatusCB& InRequesterCallback);
	void RemoveRequest(UFigmaImporter* FigmaImporter);

private:
	UPROPERTY()
	TArray<UFigmaImporter*> Requests;
};
