#pragma once
#include "StructViewerFilter.h"

class MASSGAMEPLAYSCRIPTEDITOR_API FZeroSizeStructFilter : public IStructViewerFilter
{
public:
	FZeroSizeStructFilter(){};
	virtual bool IsStructAllowed(const FStructViewerInitializationOptions& InInitOptions, const UScriptStruct* InStruct, TSharedRef<FStructViewerFilterFuncs> InFilterFuncs) override;
	virtual bool IsUnloadedStructAllowed(const FStructViewerInitializationOptions& InInitOptions, const FSoftObjectPath& InStructPath, TSharedRef<FStructViewerFilterFuncs> InFilterFuncs) override;
};