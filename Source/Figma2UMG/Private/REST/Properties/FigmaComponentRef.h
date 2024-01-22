// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FigmaDocumentationLink.h"
#include "FigmaReference.h"

#include "FigmaComponentRef.generated.h"

USTRUCT()
struct FIGMA2UMG_API FFigmaComponentRef : public FFigmaReference
{
public:
	GENERATED_BODY()

protected:
	UPROPERTY()
	FString ComponentSetId;

	UPROPERTY()
	TArray<FFigmaDocumentationLink> DocumentationLinks;
};
