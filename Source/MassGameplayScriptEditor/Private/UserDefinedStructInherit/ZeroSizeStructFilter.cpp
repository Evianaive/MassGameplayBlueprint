#include "UserDefinedStructInherit/ZeroSizeStructFilter.h"


bool FZeroSizeStructFilter::IsStructAllowed(const FStructViewerInitializationOptions& InInitOptions, const UScriptStruct* InStruct, TSharedRef<FStructViewerFilterFuncs> InFilterFuncs)
{
	// Todo The Empty Struct still takes size, dow to tell the difference between struct with single bool and them?
	if(InStruct->GetStructureSize()!=1)
		return false;
	return true;
}

bool FZeroSizeStructFilter::IsUnloadedStructAllowed(const FStructViewerInitializationOptions& InInitOptions, const FSoftObjectPath& InStructPath, TSharedRef<FStructViewerFilterFuncs> InFilterFuncs)
{
	// Unloaded UserDefinedStruct always have member
	return false;
}
