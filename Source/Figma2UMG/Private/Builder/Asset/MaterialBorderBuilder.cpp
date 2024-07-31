// Copyright 2024 Buvi Games. All Rights Reserved.


#include "MaterialBorderBuilder.h"

#include "AssetToolsModule.h"
#include "Figma2UMGModule.h"
#include "FigmaImportSubsystem.h"
#include "MaterialDomain.h"
#include "MaterialEditorUtilities.h"
#include "ObjectTools.h"
#include "PackageTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Factories/MaterialFactoryNew.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionDivide.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Materials/MaterialExpressionSubtract.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Parser/Nodes/FigmaGroup.h"
#include "Parser/Nodes/FigmaInstance.h"
#include "Parser/Nodes/FigmaNode.h"
#include "Parser/Nodes/FigmaSection.h"
#include "Parser/Nodes/Vectors/FigmaText.h"
#include "Materials/MaterialInstanceConstant.h"

void UMaterialBorderBuilder::LoadOrCreateAssets()
{
	LoadAssets();
	UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();

	static const FName NAME_AssetTools = "AssetTools";
	IAssetTools* AssetTools = nullptr;
	UMaterial* MaterialAsset = Asset;
	if (MaterialAsset == nullptr)
	{
		const FString PackagePath = UPackageTools::SanitizePackageName(Node->GetPackageNameForBuilder(this));
		const FString AssetName = "BorderMaterial";
		const FString PackageName = UPackageTools::SanitizePackageName(PackagePath + TEXT("/") + AssetName);

		UClass* AssetClass = UMaterial::StaticClass();
		const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		const FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(*PackageName, *AssetName, FString()));
		MaterialAsset = Cast<UMaterial>(AssetData.FastGetAsset(true));

		if (MaterialAsset == nullptr)
		{
			AssetTools = &FModuleManager::GetModuleChecked<FAssetToolsModule>(NAME_AssetTools).Get();
			UE_LOG_Figma2UMG(Display, TEXT("Create UAsset %s/%s of type %s"), *PackagePath, *AssetName, *AssetClass->GetDisplayNameText().ToString());
			UMaterialFactoryNew* Factory = NewObject<UMaterialFactoryNew>(UMaterialFactoryNew::StaticClass());
			MaterialAsset = Cast<UMaterial>(AssetTools->CreateAsset(AssetName, PackagePath, AssetClass, Factory, FName("Figma2UMG")));
		}
		else
		{
			UE_LOG_Figma2UMG(Display, TEXT("Loading UAsset %s/%s of type %s"), *PackagePath, *AssetName, *AssetClass->GetDisplayNameText().ToString());
		}

		Asset = MaterialAsset;
		ManageMaterial = true;
		if (Importer)
		{
			Importer->SetBorderMaterial(Asset);
		}
	}

	if (!InstanceAsset && Asset)
	{
		const FString PackagePath = UPackageTools::SanitizePackageName(Node->GetPackageNameForBuilder(this));
		float StrokeWeight = GetStrokeWeight();
		const FString AssetName = UPackageTools::SanitizePackageName("BorderMaterialInst_" + FString::SanitizeFloat(StrokeWeight, 0) + "px");
		const FString PackageName = UPackageTools::SanitizePackageName(PackagePath + TEXT("/") + AssetName);

		UClass* AssetClass = UMaterialInstanceConstant::StaticClass();
		if(!AssetTools)
		{
			AssetTools = &FModuleManager::GetModuleChecked<FAssetToolsModule>(NAME_AssetTools).Get();
		}

		UE_LOG_Figma2UMG(Display, TEXT("Create UAsset %s/%s of type %s"), *PackagePath, *AssetName, *AssetClass->GetDisplayNameText().ToString());
		UMaterialInstanceConstantFactoryNew* Factory = NewObject<UMaterialInstanceConstantFactoryNew>();
		Factory->InitialParent = Asset;
		InstanceAsset = Cast<UMaterialInstanceConstant>(AssetTools->CreateAsset(AssetName, PackagePath, AssetClass, Factory, FName("Figma2UMG")));
		ManageMaterialInstance = true;
		if (Importer)
		{
			Importer->AddBorderMaterialInstances(StrokeWeight, InstanceAsset);
		}
	}

	if (Asset && ManageMaterial)
	{
		Setup();

		Asset->SetFlags(RF_Transactional);
		Asset->Modify();
		Asset->MarkPackageDirty();
		Asset->PostEditChange();
	}

	if (InstanceAsset && ManageMaterialInstance)
	{
		SetupInstance();

		InstanceAsset->SetFlags(RF_Transactional);
		InstanceAsset->Modify();
		InstanceAsset->MarkPackageDirty();
		InstanceAsset->PostEditChange();
	}
}

void UMaterialBorderBuilder::LoadAssets()
{
	UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
	Asset = Importer ? Importer->GetBorderMaterial() : nullptr;
	float StrokeWeight = GetStrokeWeight();
	InstanceAsset = Importer ? Importer->GetBorderMaterialInstances(StrokeWeight) : nullptr;

	if (Asset && InstanceAsset)
		return;

	const FString PackagePath = UPackageTools::SanitizePackageName(Node->GetPackageNameForBuilder(this));
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	if (!Asset)
	{
		const FString AssetName = "BorderMaterial";
		const FString PackageName = UPackageTools::SanitizePackageName(PackagePath + TEXT("/") + AssetName);

		const FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(*PackageName, *AssetName, FString()));
		Asset = Cast<UMaterial>(AssetData.FastGetAsset(true));
		ManageMaterial = true;
		if(Importer)
		{
			Importer->SetBorderMaterial(Asset);
		}
	}

	if(!InstanceAsset)
	{
		const FString AssetInstanceName = UPackageTools::SanitizePackageName("BorderMaterialInst_" + FString::SanitizeFloat(GetStrokeWeight(), 0) + "px");
		const FString InstancePackageName = UPackageTools::SanitizePackageName(PackagePath + TEXT("/") + AssetInstanceName);
		const FAssetData InstanceAssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(*InstancePackageName, *AssetInstanceName, FString()));
		InstanceAsset = Cast<UMaterialInstanceConstant>(InstanceAssetData.FastGetAsset(true));
		ManageMaterialInstance = true;
		if (Importer)
		{
			Importer->AddBorderMaterialInstances(StrokeWeight, InstanceAsset);
		}
	}
}

void UMaterialBorderBuilder::Reset()
{
	Asset = nullptr;
	InstanceAsset = nullptr;
	UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
	Importer->ResetBorderMaterials();
}

void UMaterialBorderBuilder::AddPackages(TArray<UPackage*>& Packages) const
{
	Super::AddPackages(Packages);
	if(InstanceAsset)
	{
		Packages.Add(InstanceAsset->GetPackage());
	}
}

void UMaterialBorderBuilder::Setup() const
{
	Asset->MaterialDomain = MD_UI;
	if (!HasAlpha())
	{
		return;
	}

	Asset->BlendMode = BLEND_Translucent;

	FMaterialEditorUtilities::InitExpressions(Asset);	
	if (UMaterialExpression* ResultExpression = SetupMaterial())
	{
		Asset->GetEditorOnlyData()->EmissiveColor.Constant = FColor::White;

		Asset->GetEditorOnlyData()->Opacity.Connect(0, ResultExpression);

		Asset->PreEditChange(NULL);
		Asset->PostEditChange();
	}
	else
	{
		UE_LOG_Figma2UMG(Error, TEXT("[UMaterialBuilder::Setup] Node %s failed to create Gradient."), *Node->GetNodeName());
	}
}

void UMaterialBorderBuilder::SetupInstance() const
{
	if (InstanceAsset)
	{
		bool FoundStroke = false;
		const FName BorderSizeStr("BorderSize");
		const float StrokeWeight = GetStrokeWeight();

		for (FScalarParameterValue& ScalarParameterValue : InstanceAsset->ScalarParameterValues)
		{
			if(!ScalarParameterValue.IsValid())
				continue;

			if (!ScalarParameterValue.ParameterInfo.Name.IsEqual(BorderSizeStr))
				continue;

			ScalarParameterValue.ParameterValue = StrokeWeight;
			FoundStroke = true;
			break;
		}

		if(!FoundStroke)
		{
			FMaterialInstanceParameterUpdateContext UpdateContext(InstanceAsset);
			FMaterialParameterInfo ParameterInfo(BorderSizeStr);
			FMaterialParameterMetadata EditorValue;
			EditorValue.bOverride = true;
			EditorValue.Value.Type = EMaterialParameterType::Scalar;
			EditorValue.Value.Float[0] = StrokeWeight;
			UpdateContext.SetParameterValueEditorOnly(ParameterInfo, EditorValue, EMaterialSetParameterValueFlags::SetCurveAtlas);
		}
	}
}

bool UMaterialBorderBuilder::HasAlpha() const
{
	return Paint != nullptr;
}

int UMaterialBorderBuilder::GetStrokeWeight() const
{
	if (const UFigmaGroup* FigmaGroup = Cast<UFigmaGroup>(Node))
	{
		return FigmaGroup->StrokeWeight;
	}
	else if (const UFigmaInstance* FigmaInstance = Cast<UFigmaInstance>(Node))
	{
		return FigmaInstance->StrokeWeight;
	}
	else if (const UFigmaSection* FigmaSection = Cast<UFigmaSection>(Node))
	{
		return FigmaSection->StrokeWeight;
	}
	else if (const UFigmaVectorNode* FigmaVector = Cast<UFigmaVectorNode>(Node))
	{
		return FigmaVector->StrokeWeight;
	}

	return 1.0f;
}

UMaterialExpression* UMaterialBorderBuilder::SetupMaterial() const
{
	if (!Paint)
		return nullptr;

	const FName BorderSizeStr("BorderSize");
	const FName RoundRatioStr("RoundRatio");

	const FString GetUserInterfaceUVFunctionPath(TEXT("/Engine/Functions/UserInterface/GetUserInterfaceUV.GetUserInterfaceUV"));
	const FString GeneratedRoundRectFunctionPath(TEXT("/Engine/Functions/Engine_MaterialFunctions01/Gradient/GeneratedRoundRect.GeneratedRoundRect"));

	UMaterialExpressionMaterialFunctionCall* GetUserInterfaceUVFunction = SetupMaterialFunction(GetUserInterfaceUVFunctionPath, -1400.0f, -300.0f);

	UMaterialExpressionScalarParameter* BorderSizeExpression = AddParameter<UMaterialExpressionScalarParameter>(BorderSizeStr, 1.0f, GetUserInterfaceUVFunction->MaterialExpressionEditorX, GetUserInterfaceUVFunction->MaterialExpressionEditorY + 400.0f);
	UMaterialExpressionScalarParameter* RoundRatioExpression = AddParameter<UMaterialExpressionScalarParameter>(RoundRatioStr, 0.0f, BorderSizeExpression->MaterialExpressionEditorX, BorderSizeExpression->MaterialExpressionEditorY + 200.0f);

	UMaterialExpression* Result = Operation<UMaterialExpressionMultiply>(BorderSizeExpression, 0, nullptr, 0, 0.0f, 2.0f);
	Result = Operation<UMaterialExpressionSubtract>(GetUserInterfaceUVFunction, 1, Result, 0, 0.0f, 0.0f, 200.0f, -50.0f);
	Result = Operation<UMaterialExpressionDivide>(Result, 0, GetUserInterfaceUVFunction, 1);
	Result = Operation<UMaterialExpressionSubtract>(Result, 0, RoundRatioExpression, 0);

	UMaterialExpressionMaterialFunctionCall* GeneratedRoundRectFunction = SetupMaterialFunction(GeneratedRoundRectFunctionPath, Result->MaterialExpressionEditorX+200.0f, Result->MaterialExpressionEditorY);
	GeneratedRoundRectFunction->FunctionInputs[1].Input.Connect(0, Result);
	GeneratedRoundRectFunction->FunctionInputs[2].Input.Connect(0, RoundRatioExpression);
	AddConstant(100.0f, GeneratedRoundRectFunction, 3);
	
	Result = Operation<UMaterialExpressionSubtract>(nullptr, 0, GeneratedRoundRectFunction, 0, 1.0f, 0.0f, 250.0f, -45.0f);

	return Result;
}
