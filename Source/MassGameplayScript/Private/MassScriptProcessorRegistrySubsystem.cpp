// Fill out your copyright notice in the Description page of Project Settings.


#include "MassScriptProcessorRegistrySubsystem.h"

#include "MassGameplayScriptSettings.h"
#include "BlueprintClass/MassScriptProcessor.h"
#include "MassSimulationSubsystem.h"

void UMassScriptProcessorRegistrySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UMassSimulationSubsystem* SimulationSubsystem = Collection.InitializeDependency<UMassSimulationSubsystem>();
	const auto& ProcessorClasses = GetDefault<UMassGameplayScriptSettings>()->ScriptProcessorClasses;
	for(const auto& ProcessorClass:ProcessorClasses)
	{
		if(const auto Class = ProcessorClass.TryLoadClass<UMassScriptProcessor>())
		{
			Class->GetDefaultObject<UMassScriptProcessor>()->ConfigureQueries();
			SimulationSubsystem->RegisterDynamicProcessor(*Class->GetDefaultObject<UMassScriptProcessor>());
		}
	}
	Super::Initialize(Collection);
}
