// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/Nodes/Vectors/FigmaVectorNode.h"

FVector2D UFigmaVectorNode::GetAbsolutePosition() const
{
	return AbsoluteBoundingBox.GetPosition();
}

FVector2D UFigmaVectorNode::GetSize() const
{
	return AbsoluteBoundingBox.GetSize();
}