// Fill out your copyright notice in the Description page of Project Settings.


#include "Builder/ButtonBuilder.h"

#include "Figma2UMGModule.h"
#include "Components/Button.h"
#include "Parser/Nodes/FigmaComponent.h"

TObjectPtr<UWidget> FButtonBuilder::Patch(TObjectPtr<UWidget> WidgetToPatch, UObject* AssetOuter)
{
	Button = Cast<UButton>(WidgetToPatch);
	if (Button)
	{
		if (Button->GetName() != PropertyName)
		{
			if (StaticFindObject(nullptr, Button->GetOuter(), *PropertyName, true))
			{
				UE_LOG_Figma2UMG(Error, TEXT("Failt to rename %s to %s. Name already exists."), *Button->GetName(), *PropertyName);
			}
			else
			{
				Button->Rename(*PropertyName);
			}
		}
	}
	else
	{
		if (WidgetToPatch && WidgetToPatch->GetName() == PropertyName)
		{
			FString OldName = PropertyName + "_OLD";
			WidgetToPatch->Rename(*OldName);
		}
		Button = NewObject<UButton>(AssetOuter, *PropertyName);
		if (WidgetToPatch)
		{
			Button->SetContent(WidgetToPatch);
		}
	}

	if (Button && DefaultComponent)
	{
		Button->SetContent(ContainerBuilder.Patch(Button->GetContent(), AssetOuter, ""));
		FString ParentName = PropertyName;
		TObjectPtr<UPanelWidget> PanelWidget = ContainerBuilder.GetContainerWidget();
		IFigmaContainer* Container = DefaultComponent;
		Container->ForEach(IFigmaContainer::FOnEachFunction::CreateLambda([ParentName, PanelWidget](UFigmaNode& ChildNode, const int Index)
			{
				TObjectPtr<UWidget> OldWidget = PanelWidget->GetChildAt(Index);
				TObjectPtr<UWidget> NewWidget = ChildNode.PatchPreInsertWidget(OldWidget);
				if (NewWidget)
				{
					if (NewWidget != OldWidget)
					{
						PanelWidget->SetFlags(RF_Transactional);
						PanelWidget->Modify();

						UE_LOG_Figma2UMG(Display, TEXT("[Widget Insert] Parent [%s] Child [%s]."), *ParentName, *ChildNode.GetNodeName());
						if (Index < PanelWidget->GetChildrenCount())
						{
							PanelWidget->ReplaceChildAt(Index, NewWidget);
						}
						else
						{
							PanelWidget->AddChild(NewWidget);
						}
					}
				}
			}));




	}

	return Button;
}

void FButtonBuilder::SetupWidget(TObjectPtr<UWidget> Widget)
{
	Button = Cast<UButton>(Widget);
	if (!Button)
	{
		if (Widget)
		{
			UE_LOG_Figma2UMG(Error, TEXT("[FBorderBuilder::SetupWidget] Fail to setup UBorder from UWidget %s of type %s."), *Widget->GetName(), *Widget->GetClass()->GetDisplayNameText().ToString());
		}
		else
		{
			UE_LOG_Figma2UMG(Warning, TEXT("[FBorderBuilder::SetupWidget] Fail to setup UBorder from a null UWidget."));
		}
	}
	if (Button)
	{
		ContainerBuilder.SetupWidget(Button->GetContent());
	}
}

void FButtonBuilder::Reset()
{
	Button = nullptr;
}

void FButtonBuilder::SetProperty(const FString& InPropertyName, const FFigmaComponentPropertyDefinition& InDefinition)
{
	PropertyName = InPropertyName;
	PropertyDefinition = InDefinition;
}

FString FButtonBuilder::GetDefaultName() const
{
	return PropertyName + TEXT("=Default");
}

FString FButtonBuilder::GetHoveredName() const
{
	return PropertyName + TEXT("=Hovered");
}

FString FButtonBuilder::GetPressedName() const
{
	return PropertyName + TEXT("=Pressed");
}

FString FButtonBuilder::GetDisabledName() const
{
	return PropertyName + TEXT("=Disabled");
}

FString FButtonBuilder::GetFocusedName() const
{
	return PropertyName + TEXT("=Focused");
}


TObjectPtr<UButton> FButtonBuilder::GetWidget() const
{
	return Button;
}

void FButtonBuilder::PatchStyle(const UFigmaComponent* InDefaultComponent, const UFigmaComponent* HoveredComponent, const UFigmaComponent* PressedComponent, const UFigmaComponent* DisabledComponent, const UFigmaComponent* FocusedComponent) const
{
	if(!Button)
		return;

	FButtonStyle Style = Button->GetStyle();

	if (InDefaultComponent)
	{
		InDefaultComponent->SetupBrush(Style.Normal);
	}

	if (HoveredComponent)
	{
		HoveredComponent->SetupBrush(Style.Hovered);
	}

	if (PressedComponent)
	{
		PressedComponent->SetupBrush(Style.Pressed);
	}

	if (DisabledComponent)
	{
		DisabledComponent->SetupBrush(Style.Disabled);
	}

	if (FocusedComponent)
	{
		//TODO
	}

	Button->SetStyle(Style);
}

void FButtonBuilder::SetDefaultComponent(UFigmaComponent* InDefaultComponent)
{
	DefaultComponent = InDefaultComponent;
	if (DefaultComponent)
	{
		DefaultComponent->SetupLayout(ContainerBuilder);
	}
}