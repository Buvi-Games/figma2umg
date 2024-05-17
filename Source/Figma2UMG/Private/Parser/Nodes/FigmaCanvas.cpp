// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/FigmaCanvas.h"

#include "Figma2UMGModule.h"
#include "Components/CanvasPanel.h"

void UFigmaCanvas::ForEach(const IWidgetOwner::FOnEachFunction& Function)
{
	Function.ExecuteIfBound(*Canvas);
}

TObjectPtr<UWidget> UFigmaCanvas::Patch(TObjectPtr<UWidget> WidgetToPatch)
{
	Canvas = nullptr;
	if (WidgetToPatch && WidgetToPatch->GetClass() == UCanvasPanel::StaticClass())
	{
		IWidgetOwner::TryRenameWidget(GetUniqueName(), WidgetToPatch);
		UE_LOG_Figma2UMG(Display, TEXT("%s Patching Canvas "), *GetUniqueName());
		Canvas = Cast<UCanvasPanel>(WidgetToPatch);
	}
	else
	{
		UE_LOG_Figma2UMG(Display, TEXT("%s New Canvas"), *GetUniqueName());
		Canvas = IWidgetOwner::NewWidget<UCanvasPanel>(GetAssetOuter(), *GetUniqueName());
	}

	return Canvas;
}

void UFigmaCanvas::SetupWidget(TObjectPtr<UWidget> Widget)
{
	if (Widget)
	{
		UE_LOG_Figma2UMG(Display, TEXT("[SetupWidget] UFigmaCanvas %s received a UWidget %s of type %s."), *GetNodeName(), *Widget->GetName(), *Widget->GetClass()->GetDisplayNameText().ToString());
	}

	Canvas = Cast<UCanvasPanel>(Widget);
	if(!Canvas)
	{
		if (Widget)
		{
			UE_LOG_Figma2UMG(Error, TEXT("[SetupWidget] UFigmaCanvas %s fail to setup UWidget %s of type %s."), *GetNodeName(), *Widget->GetName(), *Widget->GetClass()->GetDisplayNameText().ToString());
		}
		else
		{
			UE_LOG_Figma2UMG(Warning, TEXT("[SetupWidget] UFigmaCanvas %s received a null UWidget."), *GetNodeName());
		}
	}
}

void UFigmaCanvas::Reset()
{
	Canvas = nullptr;
}

TObjectPtr<UWidget> UFigmaCanvas::GetTopWidget() const
{
	return Canvas;
}

FVector2D UFigmaCanvas::GetTopWidgetPosition() const
{
	return FVector2D::Zero();
}

TObjectPtr<UPanelWidget> UFigmaCanvas::GetContainerWidget() const
{
	return Canvas;
}

void UFigmaCanvas::PatchBinds(TObjectPtr<UWidgetBlueprint> WidgetBp) const
{
}

const FString& UFigmaCanvas::GetTransitionNodeID() const
{
	return PrototypeStartNodeID;
}

const float UFigmaCanvas::GetTransitionDuration() const
{
	return 0.0f;
}

const EFigmaEasingType UFigmaCanvas::GetTransitionEasing() const
{
	return EFigmaEasingType::LINEAR;
}