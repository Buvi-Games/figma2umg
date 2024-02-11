// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/ImageRequest.h"

#include "EditorDirectories.h"
#include "HttpModule.h"
#include "ImageCoreUtils.h"
#include "ImageUtils.h"
#include "TextureCompiler.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Interfaces/IHttpResponse.h"


void FImageRequest::StartDownload(const FOnImageRequestCompleteDelegate& Delegate)
{
	OnImageRequestCompleteDelegate = Delegate;
	Status = eRequestStatus::Processing;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FImageRequest::HandleImageDownload);
	HttpRequest->SetURL(URL);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->ProcessRequest();

}

void FImageRequest::HandleImageDownload(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (bSucceeded && HttpResponse.IsValid() && HttpResponse->GetContentLength() > 0)
	{
		const int dataSize = HttpResponse->GetContentLength();

		TArray<uint8> RawData;
		RawData.Empty(dataSize);
		RawData.AddUninitialized(dataSize);
		FMemory::Memcpy(RawData.GetData(), HttpResponse->GetContent().GetData(), dataSize);

		OnImageRawReceive.ExecuteIfBound(RawData);

		Status = eRequestStatus::Succeeded;
		OnImageRequestCompleteDelegate.ExecuteIfBound(true);

		//UTexture2D* TextureNew = FImageUtils::ImportBufferAsTexture2D(RawData);
		//if (TextureNew)
		//{
		//	UObject* DuplicatedAsset = StaticDuplicateObject(TextureNew, Texture->GetPackage(), *Texture->GetName());
		//	if (DuplicatedAsset != nullptr)
		//	{
		//		// update duplicated asset & notify asset registry
		//		if (TextureNew->HasAnyFlags(RF_Transient))
		//		{
		//			DuplicatedAsset->ClearFlags(RF_Transient);
		//			DuplicatedAsset->SetFlags(RF_Public | RF_Standalone);
		//		}
		//
		//		if (TextureNew->GetOutermost()->HasAnyPackageFlags(PKG_DisallowExport))
		//		{
		//			Texture->GetPackage()->SetPackageFlags(PKG_DisallowExport);
		//		}
		//
		//		DuplicatedAsset->MarkPackageDirty();
		//		FAssetRegistryModule::AssetCreated(DuplicatedAsset);
		//
		//		// update last save directory
		//		const FString PackagePath = FPackageName::GetLongPackagePath(DuplicatedAsset->GetPathName());
		//
		//		FEditorDirectories::Get().SetLastDirectory(ELastDirectory::NEW_ASSET, PackagePath);
		//
		//		Status = eRequestStatus::Succeeded;
		//		OnImageRequestCompleteDelegate.ExecuteIfBound(true);
		//		return;
		//	}
		//}
		//FImage Image;
		//if (FImageUtils::DecompressImage(RawData.GetData(), RawData.Num(), Image))
		//{
		//	ERawImageFormat::Type PixelFormatRawFormat;
		//	EPixelFormat PixelFormat = FImageCoreUtils::GetPixelFormatForRawImageFormat(Image.Format, &PixelFormatRawFormat);
		//
		//	Texture->SetPlatformData(new FTexturePlatformData());
		//	Texture->GetPlatformData()->SizeX = Image.SizeX;
		//	Texture->GetPlatformData()->SizeY = Image.SizeY;
		//	Texture->GetPlatformData()->SetNumSlices(1);
		//	Texture->GetPlatformData()->PixelFormat = PixelFormat;
		//
		//	int32 NumBlocksX = Image.SizeX / GPixelFormats[PixelFormat].BlockSizeX;
		//	int32 NumBlocksY = Image.SizeY / GPixelFormats[PixelFormat].BlockSizeY;
		//	FTexture2DMipMap* Mip = new FTexture2DMipMap(Image.SizeX, Image.SizeY, 1);
		//	Texture->GetPlatformData()->Mips.Add(Mip);
		//	Mip->BulkData.Lock(LOCK_READ_WRITE);
		//	Mip->BulkData.Realloc((int64)NumBlocksX * NumBlocksY * GPixelFormats[PixelFormat].BlockBytes);
		//	Mip->BulkData.Unlock();
		//
		//	Texture->bNotOfflineProcessed = true;
		//
		//	uint8* MipData = static_cast<uint8*>(Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
		//	check(MipData != nullptr);
		//	int64 MipDataSize = Texture->GetPlatformData()->Mips[0].BulkData.GetBulkDataSize();
		//
		//	FImageView MipImage(MipData, Image.SizeX, Image.SizeY, 1, PixelFormatRawFormat, Image.GammaSpace);
		//
		//	// copy into texture and convert if necessary :
		//	FImageCore::CopyImage(Image, MipImage);
		//
		//	Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
		//
		//	Texture->UpdateResource();
		//
		//	FTextureCompilingManager::Get().AddTextures({ Texture });
		//
		//	//UTexture2D* TextureNew = FImageUtils::ImportBufferAsTexture2D(RawData);		
		//
		//	Status = eRequestStatus::Succeeded;
		//	OnImageRequestCompleteDelegate.ExecuteIfBound(true);
		//
		//	return;
		//}

	}
	else
	{
		Status = eRequestStatus::Failed;
		OnImageRequestCompleteDelegate.ExecuteIfBound(false);
	}

	//if (HttpRequest)
	//{
	//	HttpRequest->OnProcessRequestComplete().Unbind();
	//}
}

void FImageRequests::AddRequest(const FString& ImageName, const FString& Id, const FOnRawImageReceiveDelegate& OnImageRawReceive)
{
	FImageRequest& Request = Requests.Emplace_GetRef();
	Request.ImageName = ImageName;
	Request.Id = Id;
	Request.OnImageRawReceive = OnImageRawReceive;
}

void FImageRequests::Reset()
{
	Requests.Reset();
}

void FImageRequests::SetURL(const FString& Id, const FString& URL)
{
	FImageRequest* FoundRequest = Requests.FindByPredicate([Id](const FImageRequest& Request) { return (Request.Id == Id); });
	if (FoundRequest)
	{
		FoundRequest->URL = URL;
	}
}

FImageRequest* FImageRequests::GetNextToDownload()
{
	return Requests.FindByPredicate([](const FImageRequest& Request) { return (Request.GetStatus() == eRequestStatus::NotStarted); });
}