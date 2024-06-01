// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Interfaces/FigmaImageRequester.h"

IFigmaImageRequester::IFigmaImageRequester()
{
	OnRawImageReceivedCB.BindRaw(this, &IFigmaImageRequester::OnRawImageReceivedBase);
}

void IFigmaImageRequester::OnRawImageReceivedBase(const TArray<uint8>& RawData)
{
	OnRawImageReceived(RawData);
}
