// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/FigmaFile.h"

void FFigmaFile::PostSerialize(const TSharedRef<FJsonObject> fileJsonObject)
{
	if(Document)
	{
		Document->PostSerialize(fileJsonObject->GetObjectField("Document").ToSharedRef());
	}
}
