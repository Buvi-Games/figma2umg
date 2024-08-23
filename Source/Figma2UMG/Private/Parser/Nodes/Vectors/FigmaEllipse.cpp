// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/Vectors/FigmaEllipse.h"
bool UFigmaEllipse::DoesSupportImageRef() const
{
	// Only Circle ca support by doing RoundCorners
	return (AbsoluteBoundingBox.Height == AbsoluteBoundingBox.Width);
}
