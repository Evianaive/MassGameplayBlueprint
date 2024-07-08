// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessorBlueprintAutoManager.h"
#include "Modules/ModuleManager.h"

class FMassGameplayBlueprintEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	TSharedPtr< FMassProcessorBlueprintAutoManager > MassProcessorBlueprintAutoManager;
};
