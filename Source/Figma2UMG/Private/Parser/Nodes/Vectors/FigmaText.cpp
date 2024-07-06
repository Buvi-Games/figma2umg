// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/Vectors/FigmaText.h"

#include "Figma2UMGModule.h"
#include "WidgetBlueprint.h"
#include "Builder/WidgetBlueprintHelper.h"
#include "Builder/Widget/TextBlockWidgetBuilder.h"
#include "Components/TextBlock.h"

void UFigmaText::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);

	PostSerializeProperty(JsonObj, "fills", Fills);
	PostSerializeProperty(JsonObj, "strokes", Strokes);

	static FString StyleStr("style");
	if (JsonObj->HasTypedField<EJson::Object>(StyleStr))
	{
		const TSharedPtr<FJsonObject> StyleJson = JsonObj->GetObjectField(StyleStr);
		Style.PostSerialize(StyleJson);
	}
}

FVector2D UFigmaText::GetAbsolutePosition() const
{
	return AbsoluteBoundingBox.GetPosition();
}

FVector2D UFigmaText::GetAbsoluteSize() const
{
	return AbsoluteBoundingBox.GetSize();
}

TScriptInterface<IWidgetBuilder> UFigmaText::CreateWidgetBuilders(bool IsRoot/*= false*/, bool AllowFrameButton/*= true*/) const
{
	UTextBlockWidgetBuilder* TextBlockWidgetBuilder = NewObject<UTextBlockWidgetBuilder>();
	TextBlockWidgetBuilder->SetNode(this);

	return TextBlockWidgetBuilder;
}

void UFigmaText::ProcessComponentPropertyReference(TObjectPtr<UWidgetBlueprint> WidgetBP, TObjectPtr<UWidget> Widget, const TPair<FString, FString>& PropertyReference) const
{
	static const FString CharactersStr("characters");
	if (PropertyReference.Key == CharactersStr)
	{
		const FBPVariableDescription* VariableDescription = WidgetBP->NewVariables.FindByPredicate([PropertyReference](const FBPVariableDescription& VariableDescription)
			{
#if (ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 3)
				return VariableDescription.VarName == PropertyReference.Value;
#else
				return VariableDescription.VarName.ToString() == PropertyReference.Value;
#endif
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

			WidgetBlueprintHelper::PatchTextBind(WidgetBP, TextBlock, *PropertyReference.Value);
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