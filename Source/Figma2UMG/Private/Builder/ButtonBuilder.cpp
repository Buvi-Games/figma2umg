// Fill out your copyright notice in the Description page of Project Settings.


#include "Builder/ButtonBuilder.h"

#include "Figma2UMGModule.h"
#include "Components/Button.h"

TObjectPtr<UWidget> FButtonBuilder::Patch(TObjectPtr<UWidget> WidgetToPatch, UObject* AssetOuter)
{
	Button = Cast<UButton>(WidgetToPatch);
	if (Button)
	{
		if (Button->GetName() != PropertyName)
		{
			if (StaticFindObject(nullptr, Button->GetOuter(), *PropertyName, true))
			{
				UE_LOG_Figma2UMG(Error, TEXT("Failt to rename %s to %s. Name already exists."), *Button->GetName(), *PropertyName);
			}
			else
			{
				Button->Rename(*PropertyName);
			}
		}
	}
	else
	{
		Button = NewObject<UButton>(AssetOuter, *PropertyName);
		if (WidgetToPatch)
		{
			Button->SetContent(WidgetToPatch);
		}
	}

	return Button;
}

void FButtonBuilder::SetupWidget(TObjectPtr<UWidget> Widget)
{
	Button = Cast<UButton>(Widget);
	if (!Button)
	{
		if (Widget)
		{
			UE_LOG_Figma2UMG(Error, TEXT("[FBorderBuilder::SetupWidget] Fail to setup UBorder from UWidget %s of type %s."), *Widget->GetName(), *Widget->GetClass()->GetDisplayNameText().ToString());
		}
		else
		{
			UE_LOG_Figma2UMG(Warning, TEXT("[FBorderBuilder::SetupWidget] Fail to setup UBorder from a null UWidget."));
		}
	}
}

void FButtonBuilder::Reset()
{
	Button = nullptr;
}

void FButtonBuilder::SetProperty(const FString& InPropertyName, const FFigmaComponentPropertyDefinition& InDefinition)
{
	PropertyName = InPropertyName;
	PropertyDefinition = InDefinition;
}

TObjectPtr<UButton> FButtonBuilder::GetWidget() const
{
	return Button;
}
