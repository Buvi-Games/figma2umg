// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FigmaDocumentationLink.h"
#include "FigmaReference.h"

#include "FigmaComponentSetRef.generated.h"

USTRUCT()
struct FIGMA2UMG_API FFigmaComponentSetRef : public FFigmaReference
{
public:
	GENERATED_BODY()

protected:
	UPROPERTY()
	TArray<FFigmaDocumentationLink> DocumentationLinks;
};
