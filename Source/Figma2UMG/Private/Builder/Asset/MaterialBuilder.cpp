// Copyright 2024 Buvi Games. All Rights Reserved.


#include "MaterialBuilder.h"

#include "AssetToolsModule.h"
#include "Figma2UMGModule.h"
#include "MaterialDomain.h"
#include "ObjectTools.h"
#include "PackageTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Editor/MaterialEditor/Public/MaterialEditingLibrary.h"
#include "Factories/MaterialFactoryNew.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "MaterialGraph/MaterialGraph.h"
#include "MaterialGraph/MaterialGraphSchema.h"
#include "Materials/MaterialExpressionComponentMask.h"
#include "Materials/MaterialExpressionCustom.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Parser/FigmaFile.h"
#include "Parser/Nodes/FigmaNode.h"

void UMaterialBuilder::LoadOrCreateAssets()
{
	UMaterialFactoryNew* Factory = NewObject<UMaterialFactoryNew>(UMaterialFactoryNew::StaticClass());

	UMaterial* MaterialAsset = Cast<UMaterial>(Asset);
	if (MaterialAsset == nullptr)
	{
		const FString PackagePath = UPackageTools::SanitizePackageName(Node->GetPackageNameForBuilder(this));
		const FString AssetName = ObjectTools::SanitizeInvalidChars(Node->GetUAssetName(), INVALID_OBJECTNAME_CHARACTERS);
		const FString PackageName = UPackageTools::SanitizePackageName(PackagePath + TEXT("/") + AssetName);

		UClass* AssetClass = UMaterial::StaticClass();
		const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		const FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(*PackageName, *AssetName, FString()));
		MaterialAsset = Cast<UMaterial>(AssetData.FastGetAsset(true));

		if (MaterialAsset == nullptr)
		{
			static const FName NAME_AssetTools = "AssetTools";
			IAssetTools* AssetTools = &FModuleManager::GetModuleChecked<FAssetToolsModule>(NAME_AssetTools).Get();
			UE_LOG_Figma2UMG(Display, TEXT("Create UAsset %s/%s of type %s"), *PackagePath, *AssetName, *AssetClass->GetDisplayNameText().ToString());
			MaterialAsset = Cast<UMaterial>(AssetTools->CreateAsset(AssetName, PackagePath, AssetClass, Factory, FName("Figma2UMG")));
		}
		else
		{

			UPackage* Pkg = CreatePackage(*PackagePath);
			const EObjectFlags Flags = RF_Public | RF_Standalone | RF_Transactional;
			UE_LOG_Figma2UMG(Display, TEXT("Reimport UAsset %s/%s of type %s"), *PackagePath, *AssetName, *AssetClass->GetDisplayNameText().ToString());
			MaterialAsset = Cast<UMaterial>(Factory->FactoryCreateNew(AssetClass, Pkg, *AssetName, Flags, nullptr, GWarn));
			if (MaterialAsset)
			{
				FAssetRegistryModule::AssetCreated(MaterialAsset);
			}
		}

		Asset = MaterialAsset;
	}

	if (MaterialAsset)
	{
		Setup();

		MaterialAsset->SetFlags(RF_Transactional);
		MaterialAsset->Modify();
	}
}

void UMaterialBuilder::LoadAssets()
{
	const FString PackagePath = UPackageTools::SanitizePackageName(Node->GetPackageNameForBuilder(this));
	const FString AssetName = ObjectTools::SanitizeInvalidChars(Node->GetUAssetName(), INVALID_OBJECTNAME_CHARACTERS);
	const FString PackageName = UPackageTools::SanitizePackageName(PackagePath + TEXT("/") + AssetName);

	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	const FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(*PackageName, *AssetName, FString()));
	Asset = Cast<UMaterial>(AssetData.FastGetAsset(true));
}

UPackage* UMaterialBuilder::GetAssetPackage() const
{
	return Asset ? Asset->GetPackage() : nullptr;
}

const TObjectPtr<UMaterial>& UMaterialBuilder::GetAsset() const
{
	return Asset;
}

void UMaterialBuilder::SetPaint(const FFigmaPaint* InPaint)
{
	Paint = InPaint;
}

void UMaterialBuilder::Setup() const
{
	Asset->MaterialDomain = MD_UI;
	if (!Asset->MaterialGraph)
	{
		Asset->MaterialGraph = CastChecked<UMaterialGraph>(FBlueprintEditorUtils::CreateNewGraph(Asset, NAME_None, UMaterialGraph::StaticClass(), UMaterialGraphSchema::StaticClass()));
	}
	Asset->MaterialGraph->Material = Asset;
	UMaterialGraph* ExpressionGraph = ToRawPtr(Asset->MaterialGraph);
	if (ExpressionGraph)
	{
		UMaterialExpression* PositionInput = SetupGradientInput(ExpressionGraph);
		UMaterialExpression* GradientExpression = SetupGradientNode(PositionInput);
		if (GradientExpression)
		{
			ExpressionGraph->AddExpression(GradientExpression, true);
			Asset->GetEditorOnlyData()->EmissiveColor.Connect(0, GradientExpression);
			Asset->MaterialGraph->RebuildGraph();
		}
		else
		{
			UE_LOG_Figma2UMG(Error, TEXT("[UMaterialBuilder::Setup] Node %s failed to create Gradient."), *Node->GetNodeName());
		}
	}
}

UMaterialExpression* UMaterialBuilder::SetupGradientInput(UMaterialGraph* ExpressionGraph) const
{
	UMaterialExpressionTextureCoordinate* TextureCoord = Cast<UMaterialExpressionTextureCoordinate>(UMaterialEditingLibrary::CreateMaterialExpressionEx(Asset, nullptr, UMaterialExpressionTextureCoordinate::StaticClass(), Asset, -1100.0f, 0.0f));
	UMaterialExpressionComponentMask* MaskR = Cast<UMaterialExpressionComponentMask>(UMaterialEditingLibrary::CreateMaterialExpressionEx(Asset, nullptr, UMaterialExpressionComponentMask::StaticClass(), Asset, -900.0f, 0.0f));
	if (TextureCoord && MaskR)
	{
		MaskR->R = 1;
		MaskR->G = 0;
		MaskR->B = 0;
		MaskR->A = 0;
		MaskR->Input.Connect(0, TextureCoord);
		ExpressionGraph->AddExpression(TextureCoord, true);
		ExpressionGraph->AddExpression(MaskR, true);
	}

	return MaskR;
}

UMaterialExpression* UMaterialBuilder::SetupGradientNode(UMaterialExpression* PositionInput) const
{
	if (!Paint)
		return nullptr;

	UMaterialExpression* GradientExpression = nullptr;
	switch (Paint->Type)
	{
	case EPaintTypes::SOLID:
		return nullptr;
	case EPaintTypes::GRADIENT_LINEAR:
		GradientExpression = SetupGradientLinearNode(PositionInput);
		break;
	case EPaintTypes::GRADIENT_RADIAL:
		break;
	case EPaintTypes::GRADIENT_ANGULAR:
		break;
	case EPaintTypes::GRADIENT_DIAMOND:
		break;
	case EPaintTypes::IMAGE:
		return nullptr;
	case EPaintTypes::EMOJI:
		return nullptr;
	case EPaintTypes::VIDEO:
		return nullptr;
	}
	return GradientExpression;
}

UMaterialExpression* UMaterialBuilder::SetupGradientLinearNode(UMaterialExpression* PositionInput) const
{
	UMaterialExpressionCustom* GradientLinearExpression = Cast<UMaterialExpressionCustom>(UMaterialEditingLibrary::CreateMaterialExpressionEx(Asset, nullptr, UMaterialExpressionCustom::StaticClass(), Asset, -700.0f, 0.0f));
	if (GradientLinearExpression)
	{
		GradientLinearExpression->OutputType == CMOT_Float4;
		if (GradientLinearExpression->Inputs.Num() == 0)
		{
			FCustomInput input;
			input.InputName = "InputPosition";
			GradientLinearExpression->Inputs.Add(input);
		}
		else
		{
			GradientLinearExpression->Inputs[0].InputName = "InputPosition";
		}

		if(Paint->GradientStops.Num() == 2)
		{
			GradientLinearExpression->Code = "float4 Color1 = float4(" + FString::SanitizeFloat(Paint->GradientStops[0].Color.R);
			GradientLinearExpression->Code += ", " + FString::SanitizeFloat(Paint->GradientStops[0].Color.G);
			GradientLinearExpression->Code += ", " + FString::SanitizeFloat(Paint->GradientStops[0].Color.B);
			GradientLinearExpression->Code += ", " + FString::SanitizeFloat(Paint->GradientStops[0].Color.A) + ");\n";

			GradientLinearExpression->Code += "float4 Color2 = float4(" + FString::SanitizeFloat(Paint->GradientStops[1].Color.R);
			GradientLinearExpression->Code += ", " + FString::SanitizeFloat(Paint->GradientStops[1].Color.G);
			GradientLinearExpression->Code += ", " + FString::SanitizeFloat(Paint->GradientStops[1].Color.B);
			GradientLinearExpression->Code += ", " + FString::SanitizeFloat(Paint->GradientStops[1].Color.A) + ");\n";

			GradientLinearExpression->Code += "if (InputPosition <= " + FString::SanitizeFloat(Paint->GradientStops[0].Position) + ")\n{\n  return Color1;\n}\n";
			GradientLinearExpression->Code += "else if (InputPosition >= " + FString::SanitizeFloat(Paint->GradientStops[1].Position) + ")\n{\n  return Color2;\n}\n";
			GradientLinearExpression->Code += "else \n{\n";
			GradientLinearExpression->Code += "  float4 ColorResult = lerp(Color1, Color2, InputPosition);\n  return ColorResult;\n}";
		}
		else
		{
			for(int i = 1; i < Paint->GradientStops.Num(); i++)
			{
				if(i == 1)
				{
					GradientLinearExpression->Code = "float4 Color1;\nfloat4 Color2;\nfloat Position = InputPosition;\n";
					GradientLinearExpression->Code += "if (InputPosition < " + FString::SanitizeFloat(Paint->GradientStops[i].Position) + ")\n{\n";
					GradientLinearExpression->Code += "  Position = InputPosition / " + FString::SanitizeFloat(Paint->GradientStops[i].Position) + ";\n";
				}
				else if (i == Paint->GradientStops.Num() - 1)
				{
					GradientLinearExpression->Code += "else\n{\n";
					GradientLinearExpression->Code += "  Position = (InputPosition - " + FString::SanitizeFloat(Paint->GradientStops[i - 1].Position) + ") / (" + FString::SanitizeFloat(Paint->GradientStops[i].Position - Paint->GradientStops[i - 1].Position) + ");\n";
				}
				else
				{
					GradientLinearExpression->Code += "else if (InputPosition < " + FString::SanitizeFloat(Paint->GradientStops[i].Position) + ")\n{\n";
					GradientLinearExpression->Code += "  Position = (InputPosition - " + FString::SanitizeFloat(Paint->GradientStops[i - 1].Position) + ") / (" + FString::SanitizeFloat(Paint->GradientStops[i].Position - Paint->GradientStops[i - 1].Position) + ");\n";
				}

				GradientLinearExpression->Code += "  Color1 = float4(" + FString::SanitizeFloat(Paint->GradientStops[i - 1].Color.R);
				GradientLinearExpression->Code += ", " + FString::SanitizeFloat(Paint->GradientStops[i - 1].Color.G);
				GradientLinearExpression->Code += ", " + FString::SanitizeFloat(Paint->GradientStops[i - 1].Color.B);
				GradientLinearExpression->Code += ", " + FString::SanitizeFloat(Paint->GradientStops[i - 1].Color.A) + ");\n";

				GradientLinearExpression->Code += "  Color2 = float4(" + FString::SanitizeFloat(Paint->GradientStops[i].Color.R);
				GradientLinearExpression->Code += ", " + FString::SanitizeFloat(Paint->GradientStops[i].Color.G);
				GradientLinearExpression->Code += ", " + FString::SanitizeFloat(Paint->GradientStops[i].Color.B);
				GradientLinearExpression->Code += ", " + FString::SanitizeFloat(Paint->GradientStops[i].Color.A) + ");\n";

				GradientLinearExpression->Code += "}\n";

			}

			GradientLinearExpression->Code += "float4 ColorResult = lerp(Color1, Color2, Position);\nreturn ColorResult;";
		}

		GradientLinearExpression->Inputs[0].Input.Connect(0, PositionInput);
	}
	return GradientLinearExpression;
}
