// Fill out your copyright notice in the Description page of Project Settings.


#include "Builder/ImageBuilder.h"

#include "Figma2UMGModule.h"
#include "Components/Image.h"
#include "Components/Widget.h"

void FImageBuilder::Reset()
{
	Image = nullptr;
}

void FImageBuilder::SetupWidget(TObjectPtr<UWidget> Widget)
{
	Image = Cast<UImage>(Widget);

	if (!Image)
	{
		if (Widget)
		{
			UE_LOG_Figma2UMG(Error, TEXT("[FImageBuilder::SetupWidget] Fail to setup UPanelWidget from UWidget %s of type %s."), *Widget->GetName(), *Widget->GetClass()->GetDisplayNameText().ToString());
		}
		else
		{
			UE_LOG_Figma2UMG(Warning, TEXT("[FImageBuilder::SetupWidget] Fail to setup UPanelWidget from a null Widget."));
		}
	}
}