#include "UserDefinedStructInherit/MassBasicStructFilter.h"


bool FMassBasicStructFilter::IsStructAllowed(const FStructViewerInitializationOptions& InInitOptions,
										const UScriptStruct* InStruct, TSharedRef<FStructViewerFilterFuncs> InFilterFuncs)
{
	if(!FZeroSizeStructFilter::IsStructAllowed(InInitOptions, InStruct, InFilterFuncs))
		return false;
	// Todo return ture if the struct fit some meta data in order to make user easier to manager their own bp struct
	static FName ModulePath = "/Script/MassEntity";
	if(!InStruct->GetName().Contains("Mass"))
		return false;	
	if(InStruct->GetStructPathName().GetPackageName()!=ModulePath)
		return false;
	
	return true;
}

bool FMassBasicStructFilter::IsUnloadedStructAllowed(const FStructViewerInitializationOptions& InInitOptions,
	const FSoftObjectPath& InStructPath, TSharedRef<FStructViewerFilterFuncs> InFilterFuncs)
{
	return false;
}
