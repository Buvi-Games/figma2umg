// Fill out your copyright notice in the Description page of Project Settings.


#include "Parser/Nodes/Vectors/FigmaVectorNode.h"

#include "REST/FigmaImporter.h"

FVector2D UFigmaVectorNode::GetAbsolutePosition() const
{
	return AbsoluteBoundingBox.GetPosition();
}

FVector2D UFigmaVectorNode::GetSize() const
{
	return AbsoluteBoundingBox.GetSize();
}

void UFigmaVectorNode::AddImageRequest(FImageRequests& ImageRequests)
{
	ImageRequests.AddRequest(GetNodeName(), GetId());
}
