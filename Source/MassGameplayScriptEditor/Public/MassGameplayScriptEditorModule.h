﻿// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FMassProcessorBlueprintAutoManager;
class FMassGameplayScriptEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	TSharedPtr< FMassProcessorBlueprintAutoManager > MassProcessorBlueprintAutoManager;
	TSharedPtr<class FUserDefinedStructEditMassListener> MassSuperStructRestore;
};
