// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/Nodes/FigmaComponent.h"

#include "Components/CanvasPanelSlot.h"
#include "REST/FigmaFile.h"
#include "REST/Properties/FigmaComponentRef.h"

FString UFigmaComponent::GetPackagePath() const
{
	TObjectPtr<UFigmaNode> TopParentNode = ParentNode;
	while (TopParentNode && TopParentNode->GetParentNode())
	{
		TopParentNode = TopParentNode->GetParentNode();
	}

	return TopParentNode->GetCurrentPackagePath() + TEXT("/") + "COMPONENTS";
}

FString UFigmaComponent::GetAssetName() const
{
	return GetUniqueName();
}

void UFigmaComponent::PostInsert(UWidget* Widget) const
{
	Super::PostInsert(Widget);
	UCanvasPanelSlot* CanvasSlot = Widget->Slot ? Cast<UCanvasPanelSlot>(Widget->Slot) : nullptr;
	if (CanvasSlot)
	{
		CanvasSlot->SetPosition(GetPosition());
	}
}

void UFigmaComponent::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);

	TObjectPtr<UFigmaFile> FigmaFile = GetFigmaFile();
	FFigmaComponentRef* ComponentRef = FigmaFile->FindComponentRef(GetId());
	ComponentRef->SetComponent(this);
}

