// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FMassScriptProcessorAutoManager;
class FMassGameplayScriptEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	TSharedPtr< FMassScriptProcessorAutoManager > MassScriptProcessorAutoManager;
	TSharedPtr<class FUserDefinedStructEditMassListener> MassSuperStructRestore;
};
