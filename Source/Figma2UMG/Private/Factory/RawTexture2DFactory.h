// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "Factories/TextureFactory.h"
#include "RawTexture2DFactory.generated.h"

/**
 * 
 */
UCLASS(hidecategories = Object, MinimalAPI)
class URawTexture2DFactory : public UTextureFactory
{
	GENERATED_UCLASS_BODY()

	/** width of new texture */
	UPROPERTY()
	TArray<uint8> RawData;

	virtual bool ShouldShowInNewMenu() const override;
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	
};
