// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/FigmaContainer.h"

void IFigmaContainer::ForEach(const FOnEachFunction& Function)
{
	TArray<UFigmaNode*>& Children = GetChildren();
	for (auto Element : Children)
	{
		if(!Element)
			continue;

		Function.Execute(*Element);
	}
}
