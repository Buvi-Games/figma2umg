// Fill out your copyright notice in the Description page of Project Settings.


#include "Factory/RawTexture2DFactory.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
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

	FImage Image;
	if (!FImageUtils::DecompressImage(RawData.GetData(), RawData.Num(), Image))
	{
		return nullptr;
	}

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	EImageFormat ImageFormat = ImageWrapperModule.DetectImageFormat(RawData.GetData(), RawData.Num());
	const FString FileExtension = GetExtensionFromFormat(ImageFormat);
	CurrentFilename = FPaths::ProjectContentDir() + TEXT("../Downloads/") + DownloadSubFolder + TEXT("/Images/") + InName.ToString() + TEXT(".") + FileExtension;

	const int64 FileSize = IFileManager::Get().FileSize(*CurrentFilename);
	const int32 Gigabyte = 1024 * 1024 * 1024;
	if (FileSize < Gigabyte)
	{
		FileHash = FMD5Hash::HashFile(*CurrentFilename);
	}

	FFileHelper::SaveArrayToFile(RawData, *CurrentFilename);

	const uint8* Ptr = RawData.GetData();

	SuppressImportOverwriteDialog();

	UObject* Object = FactoryCreateBinary(UTexture2D::StaticClass(), InParent, InName, Flags, Context, *FileExtension, Ptr, Ptr + RawData.Num(), Warn);
	UTexture2D* Texture = Cast<UTexture2D>(Object);

	CurrentFilename = TEXT("");

	return Texture;
}

FString URawTexture2DFactory::GetExtensionFromFormat(const EImageFormat& ImageFormat) const
{
	switch (ImageFormat)
	{
	case EImageFormat::PNG:
		return "png";
	case EImageFormat::JPEG:
		return "jpg";
	case EImageFormat::GrayscaleJPEG:
		return "jpg";
	case EImageFormat::BMP:
		return "bmp";
	case EImageFormat::ICO:
		return "ico";
	case EImageFormat::EXR:
		return "exr";
	case EImageFormat::ICNS:
		return "icns";
	case EImageFormat::TGA:
		return "tga";
	case EImageFormat::HDR:
		return "hdr";
	case EImageFormat::TIFF:
		return "tiff";
	case EImageFormat::DDS:
		return "dds";
	default:
		break;
	}


	return "";
}