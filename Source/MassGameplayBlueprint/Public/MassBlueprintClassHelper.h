#pragma once

class FMassBlueprintClassHelper
{
public:
	
	// If we're the blueprint skeleton class, we don't need the VM. We should be an empty vessel. Ideally there should
	// be a class flag to check for this, but there isn't, so we're left with name matching.
	static bool IsSkeletonClass(const UClass* InClass)
	{
#if WITH_EDITOR 
		return InClass && InClass->ClassGeneratedBy &&
			(InClass->GetName().StartsWith(TEXT("SKEL_")) || InClass->GetName().StartsWith(TEXT("REINST_SKEL_")));
#else
		// For non-editor targets, this is all irrelevant, since the skeleton class will never exist for them.
		return false;
#endif
	};	
};
