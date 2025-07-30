// MIT License
// Copyright (c) 2024 Buvi Games


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

void IFigmaContainer::ForEach(const FOnConstEachFunction& Function) const
{
	const TArray<UFigmaNode*>& Children = GetChildrenConst();
	for (int i = 0; i < Children.Num(); i++)
	{
		const UFigmaNode* Child = Children[i];
		if (!Child)
			continue;

		Function.Execute(*Child, i);
	}
}
