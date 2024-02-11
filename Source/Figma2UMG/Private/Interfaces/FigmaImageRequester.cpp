// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/FigmaImageRequester.h"

IFigmaImageRequester::IFigmaImageRequester()
{
	OnRawImageReceivedCB.BindRaw(this, &IFigmaImageRequester::OnRawImageReceivedBase);
}

void IFigmaImageRequester::OnRawImageReceivedBase(TArray<uint8>& RawData)
{
	OnRawImageReceived(RawData);
}
