// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/Vectors/FigmaText.h"

#include "WidgetBlueprint.h"
#include "Builder/WidgetBlueprintBuilder.h"
#include "Components/TextBlock.h"

FVector2D UFigmaText::GetAbsolutePosition() const
{
	return AbsoluteBoundingBox.GetPosition();
}

FVector2D UFigmaText::GetSize() const
{
	return AbsoluteBoundingBox.GetSize();
}

void UFigmaText::ForEach(const FOnEachFunction& Function)
{
	if (Builder.TextBlock)
	{
		Function.ExecuteIfBound(*Builder.TextBlock);
	}
}

TObjectPtr<UWidget> UFigmaText::Patch(TObjectPtr<UWidget> WidgetToPatch)
{
	Builder.TextBlock = Cast<UTextBlock>(WidgetToPatch);
	if (Builder.TextBlock)
	{
		if (Builder.TextBlock->GetName() != GetUniqueName())
		{
			IWidgetOwner::TryRenameWidget(GetUniqueName(), Builder.TextBlock);
		}
	}
	else
	{
		Builder.TextBlock = IWidgetOwner::NewWidget<UTextBlock>(GetAssetOuter(), *GetUniqueName());
	}

	Builder.TextBlock->SetText(FText::FromString(Characters));
	Builder.TextBlock->SetAutoWrapText(true);

	Builder.SetStyle(Style);
	
	if (!Fills.IsEmpty())
	{
		Builder.SetFill(Fills[0]);
	}

	return Builder.TextBlock;
}

void UFigmaText::SetupWidget(TObjectPtr<UWidget> Widget)
{
	if (Widget)
	{
		UE_LOG_Figma2UMG(Display, TEXT("[SetupWidget] UFigmaText %s received a UWidget %s of type %s."), *GetNodeName(), *Widget->GetName(), *Widget->GetClass()->GetDisplayNameText().ToString());
	}

	Builder.SetupWidget(Widget);
}

void UFigmaText::PostInsert() const
{
	TObjectPtr<UWidget> TopWidget = GetTopWidget();
	if (!TopWidget)
		return;

	IWidgetOwner::PostInsert();

	SetSize(TopWidget, AbsoluteBoundingBox.GetSize());
	SetAlign(TopWidget, Style.TextAlignHorizontal, Style.TextAlignVertical);
}

TObjectPtr<UWidget> UFigmaText::GetTopWidget() const
{
	return Builder.TextBlock;
}

FVector2D UFigmaText::GetTopWidgetPosition() const
{
	return GetPosition();
}

TObjectPtr<UPanelWidget> UFigmaText::GetContainerWidget() const
{
	return nullptr;
}

void UFigmaText::PatchBinds(TObjectPtr<UWidgetBlueprint> WidgetBp) const
{
	if (WidgetBp == nullptr)
		return;

	ProcessComponentPropertyReferences(WidgetBp, Builder.TextBlock);
}

void UFigmaText::ProcessComponentPropertyReference(TObjectPtr<UWidgetBlueprint> WidgetBP, TObjectPtr<UWidget> Widget, const TPair<FString, FString>& PropertyReference) const
{
	static const FString CharactersStr("characters");
	if (PropertyReference.Key == CharactersStr)
	{
		const FBPVariableDescription* VariableDescription = WidgetBP->NewVariables.FindByPredicate([PropertyReference](const FBPVariableDescription& VariableDescription)
			{
				return VariableDescription.VarName == PropertyReference.Value;
			});

		if (VariableDescription != nullptr)
		{
			UE_LOG_Figma2UMG(Display, TEXT("[ProcessComponentPropertyReference] Variable '%s' found in UWidgetBlueprint %s."), *PropertyReference.Value, *WidgetBP->GetName());
			TObjectPtr<UTextBlock> TextBlock = Cast<UTextBlock>(Widget);
			if (TextBlock == nullptr)
			{
				UE_LOG_Figma2UMG(Error, TEXT("[ProcessComponentPropertyReference] UWidgetBlueprint %s's Widget '%s' is not a UTextBlock. Fail to bind %s."), *WidgetBP->GetName(), *Widget->GetName(), *PropertyReference.Value);
				return;
			}

			WidgetBlueprintBuilder::PatchTextBind(WidgetBP, TextBlock, *PropertyReference.Value);
			return;
		}
		else
		{
			UClass* WidgetClass = Widget->GetClass();
			FProperty* Property = WidgetClass ? FindFProperty<FProperty>(WidgetClass, *PropertyReference.Value) : nullptr;
			if (Property)
			{
				const FStrProperty* StringProperty = CastField<FStrProperty>(Property);
				void* Value = StringProperty->ContainerPtrToValuePtr<uint8>(Widget);
				StringProperty->SetPropertyValue(Value, Characters);

				UE_LOG_Figma2UMG(Display, TEXT("[ProcessComponentPropertyReference] Variable '%s' found in UWidget %s."), *PropertyReference.Value, *Widget->GetName());
				return;
			}

		}

		UE_LOG_Figma2UMG(Error, TEXT("[ProcessComponentPropertyReference] Variable '%s' not found in UWidgetBlueprint %s or UWidget %s."), *PropertyReference.Value, *WidgetBP->GetName(), *Widget->GetName());
	}
	else
	{
		Super::ProcessComponentPropertyReference(WidgetBP, Widget, PropertyReference);
	}
}

void UFigmaText::Reset()
{
	Builder.Reset();
}