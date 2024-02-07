// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/FigmaContainer.h"

void IFigmaContainer::ForEach(const FOnEachFunction& Function)
{
	TArray<UFigmaNode*>& Children = GetChildren();
	for (int i = 0; i < Children.Num(); i++)
	{
		UFigmaNode* Child = Children[i];
		if(!Child)
			continue;

		Function.Execute(*Child, i);
	}
}
