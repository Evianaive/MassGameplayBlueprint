#pragma once
#include "StructViewerFilter.h"

class FStructEditorExtenderCreator_MassSelect
{
public:
	FStructEditorExtenderCreator_MassSelect(){};
	static void OnUserDefinedStructEditorOpen(UObject* Object, IAssetEditorInstance* EditorInstance, TSharedPtr<IStructViewerFilter> InFilter);

	static FName SuperStructMetaData;
};