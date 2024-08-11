// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/FigmaCanvas.h"

#include "Builder/Widget/Panels/CanvasBuilder.h"
#include "Builder/Widget/PanelWidgetBuilder.h"

const FString& UFigmaCanvas::GetTransitionNodeID(const FName EventName) const
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

TScriptInterface<IWidgetBuilder> UFigmaCanvas::CreateWidgetBuilders(bool IsRoot /*= false*/, bool AllowFrameButton/*= true*/) const
{
	UCanvasBuilder* Builder = NewObject<UCanvasBuilder>();
	Builder->SetNode(this);

	for (const UFigmaNode* Child : Children)
	{
		if (TScriptInterface<IWidgetBuilder> SubBuilder = Child->CreateWidgetBuilders())
		{
			Builder->AddChild(SubBuilder);
		}
	}

	return Builder;
}