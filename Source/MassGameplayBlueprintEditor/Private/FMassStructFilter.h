﻿#pragma once
#include "StructViewerFilter.h"

class FMassStructFilter : public IStructViewerFilter
{
public:
	FMassStructFilter(){};
	virtual ~FMassStructFilter() override {};
	virtual bool IsStructAllowed(const FStructViewerInitializationOptions& InInitOptions, const UScriptStruct* InStruct, TSharedRef<FStructViewerFilterFuncs> InFilterFuncs) override;
	virtual bool IsUnloadedStructAllowed(const FStructViewerInitializationOptions& InInitOptions, const FSoftObjectPath& InStructPath, TSharedRef<FStructViewerFilterFuncs> InFilterFuncs) override;
};

class FMassStructSelectExtenderCreator
{
public:
	FMassStructSelectExtenderCreator(){};
	static void OnUserDefinedStructEditorOpen(UObject* Object, IAssetEditorInstance* EditorInstance);
};