// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/Nodes/FigmaInstance.h"

#include "REST/FigmaFile.h"

//FString UFigmaInstance::GetPackagePath() const
//{
//	TObjectPtr<UFigmaNode> TopParentNode = ParentNode;
//	while (TopParentNode && TopParentNode->GetParentNode())
//	{
//		TopParentNode = TopParentNode->GetParentNode();
//	}
//
//	return TopParentNode->GetCurrentPackagePath() + TEXT("/") + "COMPONENTS";
//}
//
//FString UFigmaInstance::GetAssetName() const
//{
//	TObjectPtr<UFigmaFile> FigmaFile = GetFigmaFile();
//	const FString ComponentName = FigmaFile->FindComponentName(ComponentId);
//	return ComponentName + "_" + ComponentId;
//}

TObjectPtr<UWidget> UFigmaInstance::PatchWidgetImp(TObjectPtr<UWidget> WidgetToPatch)
{
	return WidgetToPatch;// Super::PatchWidgetImp(WidgetToPatch);
}
