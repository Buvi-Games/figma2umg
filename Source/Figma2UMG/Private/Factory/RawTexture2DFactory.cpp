// Fill out your copyright notice in the Description page of Project Settings.


#include "Factory/RawTexture2DFactory.h"

#include "ImageCoreUtils.h"
#include "ImageUtils.h"

URawTexture2DFactory::URawTexture2DFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UTexture2D::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

bool URawTexture2DFactory::ShouldShowInNewMenu() const
{
	// You may not create texture2d assets in the content browser
	return false;
}

UObject* URawTexture2DFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	// Do not create a texture with bad dimensions.
	if (RawData.IsEmpty())
	{
		return nullptr;
	}







	FImportImage Image;
	if (ImportImage(RawData.GetData(), RawData.Num(), Warn, ImportFlags, Image))
	{
		//if (Class && !UTexture2D::StaticClass()->IsChildOf(Class))
		//{
		//	//The source use to import this texture is not compatible with the
		//	//existing texture type.
		//	Warn->Logf(ELogVerbosity::Error, TEXT("Cannot import/reimport a [%s] with a UTexture2D source file"), *Class->GetName());
		//	return nullptr;
		//}

		UTexture2D* Texture = CreateTexture2D(InParent, InName, Flags);
		if (Texture)
		{
			//UE_LOG(LogEditorFactories, Display, TEXT("Image imported as : %s"), *(StaticEnum<ETextureSourceFormat>()->GetDisplayNameTextByValue(Image.Format).ToString()));

			if (Image.RawDataCompressionFormat == ETextureSourceCompressionFormat::TSCF_None)
			{
				Texture->Source.Init(
					Image.SizeX,
					Image.SizeY,
					/*NumSlices=*/ 1,
					Image.NumMips,
					Image.Format,
					Image.RawData.GetData()
				);
			}
			else
			{
				Texture->Source.InitWithCompressedSourceData(
					Image.SizeX,
					Image.SizeY,
					Image.NumMips,
					Image.Format,
					Image.RawData,
					Image.RawDataCompressionFormat
				);
			}

			Texture->CompressionSettings = Image.CompressionSettings;

			// check Texture Format before setting SRGB
			if (ERawImageFormat::GetFormatNeedsGammaSpace(FImageCoreUtils::ConvertToRawImageFormat(Image.Format)))
			{
				if (ColorSpaceMode == ETextureSourceColorSpace::Auto)
				{
					Texture->SRGB = Image.SRGB;
				}
				else if (ColorSpaceMode == ETextureSourceColorSpace::Linear)
				{
					Texture->SRGB = false;
				}
				else if (ColorSpaceMode == ETextureSourceColorSpace::SRGB)
				{
					Texture->SRGB = true;
				}
				else
				{
					check(0);
				}
			}
			else
			{
				Texture->SRGB = false;
			}
		}
		return Texture;
	}








	/*
	FImage Image;
	if (!FImageUtils::DecompressImage(RawData.GetData(), RawData.Num(), Image))
	{
		return nullptr;
	}

	CurrentFilename = FPaths::ProjectContentDir() + TEXT("../Downloads/Images/") + InName.ToString() + TEXT(".png");
	FFileHelper::SaveArrayToFile(RawData, *CurrentFilename);

	//UTexture2D* Texture = NewObject<UTexture2D>(InParent, InClass, InName, Flags);
	FString FileExtension = "png";
	const uint8* Ptr = RawData.GetData();
	
	UObject* Object = FactoryCreateBinary(UTexture2D::StaticClass(), InParent, InName, Flags, Context, *FileExtension, Ptr, Ptr + (RawData.Num() - 1), Warn);
	UTexture2D* Texture = Cast<UTexture2D>(Object);

	//ERawImageFormat::Type PixelFormatRawFormat;
	//EPixelFormat PixelFormat = FImageCoreUtils::GetPixelFormatForRawImageFormat(Image.Format, &PixelFormatRawFormat);
	//Texture->Source.Init(Image);

	//Set the source art to be white as default.
	if (Texture->Source.IsValid())
	{
		// scope MipLock
		{
			//const bool bIsPoT = FMath::IsPowerOfTwo(Image.SizeX) && FMath::IsPowerOfTwo(Image.SizeY);
			//Texture->PowerOfTwoMode = ETexturePowerOfTwoSetting::None;
			//Texture->MipGenSettings = bIsPoT ? TextureMipGenSettings::TMGS_FromTextureGroup : TextureMipGenSettings::TMGS_NoMipmaps;
			//
			//FTextureSource::FMipLock MipLock(FTextureSource::ELockState::ReadWrite, &Texture->Source, 0);
			//
			//if (MipLock.IsValid())
			//{
			//	FMemory::Memset(MipLock.Image.RawData, 255, MipLock.Image.GetImageSizeBytes());
			//}
			//Texture->CompressionSettings = TC_Default;// Image.CompressionSettings;
			//Texture->SRGB = true;// Image.bSRGB;

			//
			////If the MipGenSettings was set by the translator, we must apply it before the build
			//if (Image.MipGenSettings.IsSet())
			//{
			//	// if the source has mips we keep the mips by default, unless the user changes that
			//	Texture->MipGenSettings = Image.MipGenSettings.GetValue();
			//}
		}

		Texture->UpdateResource();
		Texture->PostEditChange();
		Texture->MarkPackageDirty();
	}*/
	return Texture;
}


{

	
}