// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/FigmaComponentSet.h"

#include "Builder/WidgetBlueprintHelper.h"
#include "Builder/Widget/ButtonWidgetBuilder.h"
#include "Builder/Widget/WidgetSwitcherBuilder.h"
#include "Builder/Widget/Panels/CanvasBuilder.h"
#include "Parser/FigmaFile.h"
#include "Parser/Properties/FigmaComponentRef.h"

void UFigmaComponentSet::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);
	GenerateFile = true;

	for (const TPair<FString, FFigmaComponentPropertyDefinition>& Property : ComponentPropertyDefinitions)
	{
		if (Property.Value.Type == EFigmaComponentPropertyType::VARIANT)
		{
			if (Property.Value.IsButton())
			{
				FString DefaultName = Property.Key + TEXT("=Default");
				FString HoveredName = Property.Key + TEXT("=Hovered");
				FString PressedName = Property.Key + TEXT("=Pressed");
				FString DisabledName = Property.Key + TEXT("=Disabled");
				FString FocusedName = Property.Key + TEXT("=Focused");

				UFigmaNode** FoundDefaultNode = Children.FindByPredicate([DefaultName](const UFigmaNode* Node) {return Node->GetNodeName().Compare(DefaultName, ESearchCase::IgnoreCase) == 0; });
				UFigmaNode** FoundHoveredNode = Children.FindByPredicate([HoveredName](const UFigmaNode* Node) {return Node->GetNodeName().Compare(HoveredName, ESearchCase::IgnoreCase) == 0; });
				UFigmaNode** FoundPressedNode = Children.FindByPredicate([PressedName](const UFigmaNode* Node) {return Node->GetNodeName().Compare(PressedName, ESearchCase::IgnoreCase) == 0; });
				UFigmaNode** FoundDisabledNode = Children.FindByPredicate([DisabledName](const UFigmaNode* Node) {return Node->GetNodeName().Compare(DisabledName, ESearchCase::IgnoreCase) == 0; });
				UFigmaNode** FoundFocusedNode = Children.FindByPredicate([FocusedName](const UFigmaNode* Node) {return Node->GetNodeName().Compare(FocusedName, ESearchCase::IgnoreCase) == 0; });

				if (UFigmaComponent* DefaultComponent = FoundDefaultNode ? Cast<UFigmaComponent>(*FoundDefaultNode) : nullptr)
				{
					DefaultComponent->SetGenerateFile(false);
				}
				if (UFigmaComponent* HoveredComponent = FoundHoveredNode ? Cast<UFigmaComponent>(*FoundHoveredNode) : nullptr)
				{
					HoveredComponent->SetGenerateFile(false);
				}
				if (UFigmaComponent* PressedComponent = FoundPressedNode ? Cast<UFigmaComponent>(*FoundPressedNode) : nullptr)
				{
					PressedComponent->SetGenerateFile(false);
				}
				if (UFigmaComponent* DisabledComponent = FoundDisabledNode ? Cast<UFigmaComponent>(*FoundDisabledNode) : nullptr)
				{
					DisabledComponent->SetGenerateFile(false);
				}
				if (UFigmaComponent* FocusedComponent = FoundFocusedNode ? Cast<UFigmaComponent>(*FoundFocusedNode) : nullptr)
				{
					FocusedComponent->SetGenerateFile(false);
				}
			}
		}
		else
		{
			for (UFigmaNode* Child : Children)
			{
				UFigmaComponent* ChildComponent = Cast<UFigmaComponent>(Child);
				if (ChildComponent)
				{
					ChildComponent->TryAddComponentPropertyDefinition(Property.Key, Property.Value);
				}
			}
		}
	}

	TObjectPtr<UFigmaFile> FigmaFile = GetFigmaFile();
	FFigmaComponentSetRef* ComponentSetRef = FigmaFile->FindComponentSetRef(GetId());
	ComponentSetRef->SetComponentSet(this);
}

TScriptInterface<IWidgetBuilder> UFigmaComponentSet::CreateWidgetBuilders(bool IsRoot/*= false*/, bool AllowFrameButton/*= true*/) const
{
	if (IsRoot)
	{
		UWidgetSwitcherBuilder* WidgetSwitcherBuilder = nullptr;
		UButtonWidgetBuilder* ButtonBuilder = nullptr;
		for (const TPair<FString, FFigmaComponentPropertyDefinition>& Property : ComponentPropertyDefinitions)
		{
			if (Property.Value.Type == EFigmaComponentPropertyType::VARIANT)
			{
				if (Property.Value.IsButton())
				{
					ButtonBuilder = NewObject<UButtonWidgetBuilder>();
					ButtonBuilder->SetNode(this);

					FString DefaultName = Property.Key + TEXT("=Default");
					FString HoveredName = Property.Key + TEXT("=Hovered");
					FString PressedName = Property.Key + TEXT("=Pressed");
					FString DisabledName = Property.Key + TEXT("=Disabled");
					FString FocusedName = Property.Key + TEXT("=Focused");

					const UFigmaNode* const* FoundDefaultNode = Children.FindByPredicate([DefaultName](const UFigmaNode* Node) {return Node->GetNodeName().Compare(DefaultName, ESearchCase::IgnoreCase) == 0; });
					const UFigmaNode* const* FoundHoveredNode = Children.FindByPredicate([HoveredName](const UFigmaNode* Node) {return Node->GetNodeName().Compare(HoveredName, ESearchCase::IgnoreCase) == 0; });
					const UFigmaNode* const* FoundPressedNode = Children.FindByPredicate([PressedName](const UFigmaNode* Node) {return Node->GetNodeName().Compare(PressedName, ESearchCase::IgnoreCase) == 0; });
					const UFigmaNode* const* FoundDisabledNode = Children.FindByPredicate([DisabledName](const UFigmaNode* Node) {return Node->GetNodeName().Compare(DisabledName, ESearchCase::IgnoreCase) == 0; });
					const UFigmaNode* const* FoundFocusedNode = Children.FindByPredicate([FocusedName](const UFigmaNode* Node) {return Node->GetNodeName().Compare(FocusedName, ESearchCase::IgnoreCase) == 0; });

					ButtonBuilder->SetDefaultNode(FoundDefaultNode ? Cast<UFigmaComponent>(*FoundDefaultNode) : nullptr);
					ButtonBuilder->SetHoveredNode(FoundHoveredNode ? Cast<UFigmaComponent>(*FoundHoveredNode) : nullptr);
					ButtonBuilder->SetPressedNode(FoundPressedNode ? Cast<UFigmaComponent>(*FoundPressedNode) : nullptr);
					ButtonBuilder->SetDisabledNode(FoundDisabledNode ? Cast<UFigmaComponent>(*FoundDisabledNode) : nullptr);
					ButtonBuilder->SetFocusedNode(FoundFocusedNode ? Cast<UFigmaComponent>(*FoundFocusedNode) : nullptr);
				}
				else
				{
					WidgetSwitcherBuilder = NewObject<UWidgetSwitcherBuilder>();
					WidgetSwitcherBuilder->SetNode(this);
					for (const UFigmaNode* Child : Children)
					{
						if (!Child)
							continue;

						if (TScriptInterface<IWidgetBuilder> SubBuilder = Child->CreateWidgetBuilders())
						{
							WidgetSwitcherBuilder->AddChild(SubBuilder);
						}

					}
				}
			}
		}

		if(WidgetSwitcherBuilder)
		{
			return WidgetSwitcherBuilder;
		}

		return ButtonBuilder;
	}
	else
	{
		bool IsButton = false;
		for (const TPair<FString, FFigmaComponentPropertyDefinition>& Property : ComponentPropertyDefinitions)
		{
			if (Property.Value.Type == EFigmaComponentPropertyType::VARIANT && Property.Value.IsButton())
			{
				IsButton = true;
				break;
			}
		}

		if (IsButton)
		{
			return nullptr;
		}
		else
		{
			UCanvasBuilder* InPlaceCanvasBuilder = NewObject<UCanvasBuilder>();
			InPlaceCanvasBuilder->SetNode(this);

			for (const UFigmaNode* Child : Children)
			{
				if (!Child)
					continue;

				if (TScriptInterface<IWidgetBuilder> SubBuilder = Child->CreateWidgetBuilders())
				{
					InPlaceCanvasBuilder->AddChild(SubBuilder);
				}
			}

			return InPlaceCanvasBuilder;
		}

	}
}

{
	TObjectPtr<UFigmaNode> TopParentNode = ParentNode;
	while (TopParentNode && TopParentNode->GetParentNode())
	{
		TopParentNode = TopParentNode->GetParentNode();
	}

	return TopParentNode->GetCurrentPackagePath() + TEXT("/") + "Components";
}