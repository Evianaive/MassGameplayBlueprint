// Copyright Epic Games, Inc. All Rights Reserved.

#include "MassGameplayBlueprintEditor.h"
#include "MassProcessorBlueprintAutoManager.h"
#include "FMassStructFilter.h"
#include "Kismet2/StructureEditorUtils.h"

#define LOCTEXT_NAMESPACE "FMassGameplayBlueprintEditorModule"

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

void FMassGameplayBlueprintEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	MassProcessorBlueprintAutoManager = MakeShareable(new FMassProcessorBlueprintAutoManager);
	
	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()
	->OnAssetOpenedInEditor()
	.AddStatic(FMassStructSelectExtenderCreator::OnUserDefinedStructEditorOpen);
	
	// FStructureEditorUtils::FStructEditorManager::Get().AddListener();
	MassSuperStructRestore = MakeShareable(new FUserDefinedStructEditMassListener);
}

void FMassGameplayBlueprintEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMassGameplayBlueprintEditorModule, MassGameplayBlueprintEditor)