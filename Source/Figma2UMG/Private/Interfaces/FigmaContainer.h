// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "FigmaContainer.generated.h"

class UFigmaNode;

UINTERFACE(BlueprintType, Experimental, meta = (CannotImplementInterfaceInBlueprint))
class FIGMA2UMG_API UFigmaContainer : public UInterface
{
	GENERATED_BODY()
};

class FIGMA2UMG_API IFigmaContainer
{
	GENERATED_BODY()
public:

	DECLARE_DELEGATE_TwoParams(FOnEachFunction, UFigmaNode&, const int)
	void ForEach(const FOnEachFunction& Function);

	DECLARE_DELEGATE_TwoParams(FOnConstEachFunction, const UFigmaNode&, const int)
	void ForEach(const FOnConstEachFunction& Function) const;

	template<class NodeType>
	void GetAllChildrenByType(TArray<NodeType*>& AllFiles);

	UFUNCTION()
	virtual FString GetJsonArrayName() const = 0;

	UFUNCTION()
	virtual TArray<UFigmaNode*>& GetChildren() = 0;

	UFUNCTION()
	virtual const TArray<UFigmaNode*>& GetChildrenConst() const = 0;
};

template <class NodeType>
void IFigmaContainer::GetAllChildrenByType(TArray<NodeType*>& AllFiles)
{
	TArray<UFigmaNode*>& Children = GetChildren();
	for (UFigmaNode* Node : Children)
	{
		if (!Node)
			continue;

		if (NodeType* FigmaFileHandle = Cast<NodeType>(Node))
		{
			AllFiles.Add(FigmaFileHandle);
		}

		if (IFigmaContainer* Container = Cast<IFigmaContainer>(Node))
		{
			Container->GetAllChildrenByType(AllFiles);
		}
	}
}
