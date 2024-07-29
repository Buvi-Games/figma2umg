// Copyright 2024 Buvi Games. All Rights Reserved.


#include "ButtonWidgetBuilder.h"

#include "BlueprintDelegateNodeSpawner.h"
#include "Figma2UMGModule.h"
#include "FigmaImportSubsystem.h"
#include "K2Node_CallDelegate.h"
#include "K2Node_ComponentBoundEvent.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/ContentWidget.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"


void UButtonWidgetBuilder::PatchAndInsertWidget(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UWidget>& WidgetToPatch)
{
	Widget = Cast<UButton>(WidgetToPatch);
	const FString NodeName = Node->GetNodeName();
	const FString WidgetName = Node->GetUniqueName();
	if (Widget)
	{
		UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
		UClass* ClassOverride = Importer ? Importer->GetOverrideClassForNode<UBorder>(NodeName) : nullptr;
		if (ClassOverride && Widget->GetClass() != ClassOverride)
		{
			UButton* NewButton = UFigmaImportSubsystem::NewWidget<UButton>(WidgetBlueprint->WidgetTree, NodeName , WidgetName, ClassOverride);
			NewButton->SetContent(Widget->GetContent());
			Widget = NewButton;
		}
		UFigmaImportSubsystem::TryRenameWidget(WidgetName, Widget);
	}
	else
	{
		Widget = UFigmaImportSubsystem::NewWidget<UButton>(WidgetBlueprint->WidgetTree, NodeName, WidgetName);
		if (WidgetToPatch)
		{
			Widget->SetContent(WidgetToPatch);
		}
	}

	Insert(WidgetBlueprint->WidgetTree, WidgetToPatch, Widget);

	Setup(WidgetBlueprint);

	PatchAndInsertChild(WidgetBlueprint, Widget);
}

void UButtonWidgetBuilder::PostInsertWidgets(TObjectPtr<UWidgetBlueprint> WidgetBlueprint)
{
}

void UButtonWidgetBuilder::PatchWidgetBinds(const TObjectPtr<UWidgetBlueprint>& WidgetBlueprint)
{
	PatchEvents(WidgetBlueprint);

	Super::PatchWidgetBinds(WidgetBlueprint);
}

void UButtonWidgetBuilder::SetDefaultNode(const UFigmaGroup* InNode)
{
	DefaultNode = InNode;
	if(DefaultNode)
	{
		TScriptInterface<IWidgetBuilder> Bulder = DefaultNode->CreateWidgetBuilders(false, false);
		if(Bulder)
		{
			SetChild(Bulder);
		}
	}
}

void UButtonWidgetBuilder::SetHoveredNode(const UFigmaGroup* InNode)
{
	HoveredNode = InNode;
}

void UButtonWidgetBuilder::SetPressedNode(const UFigmaGroup* InNode)
{
	PressedNode = InNode;
}

void UButtonWidgetBuilder::SetDisabledNode(const UFigmaGroup* InNode)
{
	DisabledNode = InNode;
}

void UButtonWidgetBuilder::SetFocusedNode(const UFigmaGroup* InNode)
{
	FocusedNode = InNode;
}

void UButtonWidgetBuilder::SetWidget(const TObjectPtr<UWidget>& InWidget)
{
	Widget = Cast<UButton>(InWidget);
	SetChildWidget(Widget);
}

void UButtonWidgetBuilder::ResetWidget()
{
	Widget = nullptr;
	Super::ResetWidget();
}

TObjectPtr<UContentWidget> UButtonWidgetBuilder::GetContentWidget() const
{
	return Widget;
}

void UButtonWidgetBuilder::GetPaddingValue(FMargin& Padding) const
{
	Padding.Left = 0.0f;
	Padding.Right = 0.0f;
	Padding.Top = 0.0f;
	Padding.Bottom = 0.0f;
}

void UButtonWidgetBuilder::Setup(TObjectPtr<UWidgetBlueprint> WidgetBlueprint) const
{
	FButtonStyle Style = Widget->GetStyle();

	if (DefaultNode)
	{
		SetupBrush(Style.Normal, *DefaultNode);
		const FMargin Padding(0.0f);// = DefaultNode->GetPadding();
		Style.SetNormalPadding(Padding);
	}

	if (HoveredNode)
	{
		SetupBrush(Style.Hovered, *HoveredNode);
	}

	if (PressedNode)
	{
		SetupBrush(Style.Pressed, *PressedNode);
		const FMargin Padding(0.0f);// = PressedNode->GetPadding();
		Style.SetPressedPadding(Padding);
	}

	if (DisabledNode)
	{
		SetupBrush(Style.Disabled, *DisabledNode);
	}

	if (FocusedNode)
	{
		//TODO
	}

	Widget->SetStyle(Style);

	const FName OnButtonClicked("OnButtonClicked");
	SetupEventDispatchers(WidgetBlueprint, OnButtonClicked);

	const FName OnButtonPressed("OnButtonPressed");
	SetupEventDispatchers(WidgetBlueprint, OnButtonPressed);

	const FName OnButtonReleased("OnButtonReleased");
	SetupEventDispatchers(WidgetBlueprint, OnButtonReleased);

	const FName OnButtonHovered("OnButtonHovered");
	SetupEventDispatchers(WidgetBlueprint, OnButtonHovered);

	const FName OnButtonUnHovered("OnButtonUnHovered");
	SetupEventDispatchers(WidgetBlueprint, OnButtonUnHovered);
}

void UButtonWidgetBuilder::SetupEventDispatchers(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const FName& EventName) const
{
	if (UObject* ExistingObject = FindObject<UObject>(WidgetBlueprint, *(EventName.ToString())))
	{
		return;
	}

	FEdGraphPinType DelegateType;
	DelegateType.PinCategory = UEdGraphSchema_K2::PC_MCDelegate;
	const bool bVarCreatedSuccess = FBlueprintEditorUtils::AddMemberVariable(WidgetBlueprint, EventName, DelegateType);
	if (!bVarCreatedSuccess)
	{
		//		LogSimpleMessage(LOCTEXT("AddDelegateVariable_Error", "Adding new delegate variable failed."));
		return;
	}

	UEdGraph* const NewGraph = FBlueprintEditorUtils::CreateNewGraph(WidgetBlueprint, EventName, UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
	if (!NewGraph)
	{
		FBlueprintEditorUtils::RemoveMemberVariable(WidgetBlueprint, EventName);
	//	LogSimpleMessage(LOCTEXT("AddDelegateVariable_Error", "Adding new delegate variable failed."));
		return;
	}

	const int32 VarIndex = FBlueprintEditorUtils::FindNewVariableIndex(WidgetBlueprint, EventName);
	if (VarIndex != INDEX_NONE && WidgetBlueprint->NewVariables[VarIndex].Category.EqualTo(UEdGraphSchema_K2::VR_DefaultCategory))
	{
		static const FText FigmaCategory(FText::FromString("Figma"));
		WidgetBlueprint->NewVariables[VarIndex].Category = FigmaCategory;
	}

	NewGraph->bEditable = false;

	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	K2Schema->CreateDefaultNodesForGraph(*NewGraph);
	K2Schema->CreateFunctionGraphTerminators(*NewGraph, (UClass*)nullptr);
	K2Schema->AddExtraFunctionFlags(NewGraph, (FUNC_BlueprintCallable | FUNC_BlueprintEvent | FUNC_Public));
	K2Schema->MarkFunctionEntryAsEditable(NewGraph, true);

	WidgetBlueprint->DelegateSignatureGraphs.Add(NewGraph);
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(WidgetBlueprint);
}

void UButtonWidgetBuilder::PatchEvents(const TObjectPtr<UWidgetBlueprint>& WidgetBlueprint)
{
	FObjectProperty* VariableProperty = FindFProperty<FObjectProperty>(WidgetBlueprint->SkeletonGeneratedClass, *Widget->GetName());

	if (VariableProperty)
	{
		const FName OnClicked("OnClicked");
		const FName OnButtonClicked("OnButtonClicked");
		PatchEvent(WidgetBlueprint, VariableProperty, OnClicked, OnButtonClicked);

		const FName OnPressed("OnPressed");
		const FName OnButtonPressed("OnButtonPressed");
		PatchEvent(WidgetBlueprint, VariableProperty, OnPressed, OnButtonPressed);

		const FName OnReleased("OnReleased");
		const FName OnButtonReleased("OnButtonReleased");
		PatchEvent(WidgetBlueprint, VariableProperty, OnReleased, OnButtonReleased);

		const FName OnHovered("OnHovered");
		const FName OnButtonHovered("OnButtonHovered");
		PatchEvent(WidgetBlueprint, VariableProperty, OnHovered, OnButtonHovered);

		const FName OnUnHovered("OnUnHovered");
		const FName OnButtonUnHovered("OnButtonUnHovered");
		PatchEvent(WidgetBlueprint, VariableProperty, OnUnHovered, OnButtonUnHovered);
	}
}

void UButtonWidgetBuilder::PatchEvent(const TObjectPtr<UWidgetBlueprint>& WidgetBlueprint, FObjectProperty* VariableProperty, const FName& EventName, const FName& EventDispatchersName)
{
	const UK2Node_ComponentBoundEvent* ExistingNode = FKismetEditorUtilities::FindBoundEventForComponent(WidgetBlueprint, EventName, VariableProperty->GetFName());
	if (ExistingNode == nullptr)
	{
		FKismetEditorUtilities::CreateNewBoundEventForClass(Widget->GetClass(), EventName, WidgetBlueprint, VariableProperty);
		ExistingNode = FKismetEditorUtilities::FindBoundEventForComponent(WidgetBlueprint, EventName, VariableProperty->GetFName());
	}

	FVector2D StartPos(ExistingNode->NodePosX, ExistingNode->NodePosY);
	UEdGraphPin* ThenPin = ExistingNode->FindPin(UEdGraphSchema_K2::PN_Then);
	while (ThenPin && !ThenPin->LinkedTo.IsEmpty())
	{
		UEdGraphNode* ConnectedNode = ThenPin->LinkedTo[0]->GetOwningNode();
		if (!ConnectedNode)
		{
			break;
		}
		else if (ConnectedNode->IsA<UK2Node_CallDelegate>())
		{
			UK2Node_CallDelegate* CallFunctionNode = Cast<UK2Node_CallDelegate>(ConnectedNode);
			if (Widget->GetName().Contains(CallFunctionNode->DelegateReference.GetMemberName().ToString()))
			{
				//Nothing to do.
				return;
			}
		}

		ThenPin = ConnectedNode->FindPin(UEdGraphSchema_K2::PN_Then);
		StartPos = FVector2D(ConnectedNode->NodePosX, ConnectedNode->NodePosY);
	}

	FMulticastInlineDelegateProperty* DispatcherProperty = FindFProperty<FMulticastInlineDelegateProperty>(WidgetBlueprint->SkeletonGeneratedClass, EventDispatchersName);
	if(!DispatcherProperty)
	{
		return;
	}

	bool const bIsDelegate = DispatcherProperty->IsA(FMulticastDelegateProperty::StaticClass());
	if (bIsDelegate)
	{
		FMulticastDelegateProperty* DelegateProperty = CastFieldChecked<FMulticastDelegateProperty>(DispatcherProperty);

		UBlueprintNodeSpawner* CallSpawner = UBlueprintDelegateNodeSpawner::Create(UK2Node_CallDelegate::StaticClass(), DelegateProperty);
		TSet<FBindingObject> Bindings;
		Bindings.Add(FBindingObject(WidgetBlueprint));
		const FVector2D Offset = FVector2D(300.0f, 0.0f);
		const FVector2D NodeLocation = StartPos + Offset;
		const UEdGraphNode* GraphNode = CallSpawner->Invoke(ExistingNode->GetGraph(), Bindings, NodeLocation);
		if (GraphNode)
		{
			UEdGraphPin* ExecutePin = GraphNode->FindPin(UEdGraphSchema_K2::PN_Execute);
			if (ThenPin && ExecutePin)
			{
				ThenPin->MakeLinkTo(ExecutePin);
			}
		}
	}
}

void UButtonWidgetBuilder::SetupBrush(FSlateBrush& Brush, const UFigmaGroup& FigmaGroup) const
{

	if (!FigmaGroup.Fills.IsEmpty() && FigmaGroup.Fills[0].Visible)
	{
		Brush.TintColor = FigmaGroup.Fills[0].GetLinearColor();
	}
	else
	{
		Brush.TintColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.0f);
	}

	if (!FigmaGroup.Strokes.IsEmpty())
	{
		Brush.OutlineSettings.Color = FigmaGroup.Strokes[0].GetLinearColor();
		Brush.OutlineSettings.Width = FigmaGroup.StrokeWeight;
	}
	else
	{
		Brush.OutlineSettings.Color = FLinearColor(1.0f, 1.0f, 1.0f, 0.0f);
		Brush.OutlineSettings.Width = 0.0f;

	}

	FVector4 Corners = FigmaGroup.RectangleCornerRadii.Num() == 4 ? FVector4(FigmaGroup.RectangleCornerRadii[0], FigmaGroup.RectangleCornerRadii[1], FigmaGroup.RectangleCornerRadii[2], FigmaGroup.RectangleCornerRadii[3])
																  : FVector4(FigmaGroup.CornerRadius, FigmaGroup.CornerRadius, FigmaGroup.CornerRadius, FigmaGroup.CornerRadius);
	Brush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
	Brush.OutlineSettings.CornerRadii = Corners;
	Brush.DrawAs = ESlateBrushDrawType::RoundedBox;
}
