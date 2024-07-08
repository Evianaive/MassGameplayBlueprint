// Copyright Epic Games, Inc. All Rights Reserved.

#include "MassGameplayBlueprintEditor.h"

#include "MassProcessorBlueprintAutoManager.h"

#define LOCTEXT_NAMESPACE "FMassGameplayBlueprintEditorModule"

void FMassGameplayBlueprintEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	MassProcessorBlueprintAutoManager = MakeShareable(new FMassProcessorBlueprintAutoManager);
	
}

void FMassGameplayBlueprintEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMassGameplayBlueprintEditorModule, MassGameplayBlueprintEditor)