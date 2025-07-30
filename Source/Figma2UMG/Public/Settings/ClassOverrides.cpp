// MIT License
// Copyright (c) 2024 Buvi Games

#include "Settings/ClassOverrides.h"

bool FWidgetOverride::Match(const FString& NodeName) const
{
	if (!HasCondition)
	{
		return true;
	}
	else if (StringCheckType == EOverrideConditionCheck::StartsWith)
	{
		return NodeName.StartsWith(NameComparison);		
	}
	else if (StringCheckType == EOverrideConditionCheck::Contains)
	{
		return NodeName.Contains(NameComparison);
	}
	else if (StringCheckType == EOverrideConditionCheck::WildCard)
	{
		return NodeName.MatchesWildcard(NameComparison);
	}

	return false;
}
