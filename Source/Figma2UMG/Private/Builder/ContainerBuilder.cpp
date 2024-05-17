// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Builder/ContainerBuilder.h"

#include "Figma2UMGModule.h"
#include "Components/CanvasPanel.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Components/WrapBox.h"

void FContainerBuilder::ForEach(const IWidgetOwner::FOnEachFunction& Function)
{
	if (!Function.IsBound())
		return;

	if (Conainter)
	{
		Function.ExecuteIfBound(*Conainter);
	}
}

void FContainerBuilder::SetLayout(EFigmaLayoutMode InLayoutMode, EFigmaLayoutWrap InLayoutWrap)
{
	LayoutMode = InLayoutMode;
	LayoutWrap = InLayoutWrap;
}

TObjectPtr<UWidget> FContainerBuilder::Patch(TObjectPtr<UWidget> WidgetToPatch, UObject* AssetOuter, const FString& WidgetName)
{
	Conainter = nullptr;
	
	switch (LayoutMode)
	{
	case EFigmaLayoutMode::NONE:
	{
		Patch<UCanvasPanel>(WidgetToPatch, AssetOuter, WidgetName);
	}
	break;
	case EFigmaLayoutMode::HORIZONTAL:
	{
		if(LayoutWrap == EFigmaLayoutWrap::NO_WRAP)
		{
			Patch<UHorizontalBox>(WidgetToPatch, AssetOuter, WidgetName);
		}
		else
		{
			TObjectPtr<UWrapBox> WrapBox = Patch<UWrapBox>(WidgetToPatch, AssetOuter, WidgetName);
			WrapBox->SetOrientation(EOrientation::Orient_Horizontal);
		}
	}
	break;
	case EFigmaLayoutMode::VERTICAL:
	{
		if (LayoutWrap == EFigmaLayoutWrap::NO_WRAP)
		{
			Patch<UVerticalBox>(WidgetToPatch, AssetOuter, WidgetName);
		}
		else
		{
			TObjectPtr<UWrapBox> WrapBox = Patch<UWrapBox>(WidgetToPatch, AssetOuter, WidgetName);
			WrapBox->SetOrientation(EOrientation::Orient_Vertical);
		}
	}
	break;
	}

	return Conainter;

}

void FContainerBuilder::SetupWidget(TObjectPtr<UWidget> Widget)
{
	Conainter = Cast<UPanelWidget>(Widget);

	if(!Conainter)
	{
		if (Widget)
		{
			UE_LOG_Figma2UMG(Error, TEXT("[FContainerBuilder::SetupWidget] Fail to setup UPanelWidget from UWidget %s of type %s."), *Widget->GetName(), *Widget->GetClass()->GetDisplayNameText().ToString());
		}
		else
		{
			UE_LOG_Figma2UMG(Warning, TEXT("[FContainerBuilder::SetupWidget] Fail to setup UPanelWidget from a null Widget."));
		}
	}
}

void FContainerBuilder::Reset()
{
	Conainter = nullptr;
}

TObjectPtr<UPanelWidget> FContainerBuilder::GetContainerWidget() const
{
	return Conainter;
}