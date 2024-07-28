#pragma once
#include "ZeroSizeStructFilter.h"

class MASSGAMEPLAYSCRIPTEDITOR_API FMassBasicStructFilter : public FZeroSizeStructFilter
{
public:
	FMassBasicStructFilter(){};
	virtual ~FMassBasicStructFilter() override {};
	virtual bool IsStructAllowed(const FStructViewerInitializationOptions& InInitOptions, const UScriptStruct* InStruct, TSharedRef<FStructViewerFilterFuncs> InFilterFuncs) override;
	virtual bool IsUnloadedStructAllowed(const FStructViewerInitializationOptions& InInitOptions, const FSoftObjectPath& InStructPath, TSharedRef<FStructViewerFilterFuncs> InFilterFuncs) override;
};
