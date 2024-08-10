// Copyright Epic Games, Inc. All Rights Reserved.

#include "MassGameplayScriptEditorModule.h"
#include "MassProcessorBlueprintAutoManager.h"
#include "UserDefinedStructInherit/StructInheritExtender.h"
#include "UserDefinedStructInherit/MassBasicStructFilter.h"
#include "Customization/MassStructSelectDetails.h"
#include "Kismet2/StructureEditorUtils.h"

#define LOCTEXT_NAMESPACE "FMassGameplayScriptEditorModule"

class FUserDefinedStructEditMassListener : public FStructureEditorUtils::FStructEditorManager::ListenerType
{
public:
	virtual void PreChange(
		const UUserDefinedStruct* Changed,
		FStructureEditorUtils::EStructureEditorChangeInfo ChangedType) override
	{
		StructMap.Add(const_cast<UUserDefinedStruct*>(Changed),Changed->GetSuperStruct());
	}
	virtual void PostChange(
		const UUserDefinedStruct* Changed,
		FStructureEditorUtils::EStructureEditorChangeInfo ChangedType) override
	{
		if(auto Restore = StructMap.Find(Changed))
		{
			StructMap.Remove(Changed);
			const_cast<UUserDefinedStruct*>(Changed)->SetSuperStruct(*Restore);
		}
	}
	TMap<UUserDefinedStruct*,UStruct*> StructMap;
};

void FMassGameplayScriptEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	{
		MassProcessorBlueprintAutoManager = MakeShareable(new FMassProcessorBlueprintAutoManager);
	}	
	{
		TSharedPtr<IStructViewerFilter> StructFilter = MakeShared<FMassBasicStructFilter>();		
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()
		->OnAssetOpenedInEditor()
		.AddStatic(&FStructEditorExtenderCreator_MassSelect::OnUserDefinedStructEditorOpen,StructFilter);
	
		// FStructureEditorUtils::FStructEditorManager::Get().AddListener();
		MassSuperStructRestore = MakeShareable(new FUserDefinedStructEditMassListener);
		
		// Allow save package meta data to UUserDefinedStruct FAssetData TagValues
		UObject::GetMetaDataTagsForAssetRegistry().Add(FStructEditorExtenderCreator_MassSelect::SuperStructMetaData);
	}
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.RegisterCustomPropertyTypeLayout("MassStructSelect",
			FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FMassStructSelectDetails::MakeInstance));
	}
}

void FMassGameplayScriptEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMassGameplayScriptEditorModule, MassGameplayScriptEditor)