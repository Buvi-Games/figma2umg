// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/Nodes/FigmaInstance.h"

FString UFigmaInstance::GetPackagePath() const
{
	if (ParentNode)
	{
		return ParentNode->GetCurrentPackagePath();
	}

	return GetCurrentPackagePath();
}

FString UFigmaInstance::GetAssetName() const
{
	return GetUniqueName();
}