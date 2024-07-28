// Fill out your copyright notice in the Description page of Project Settings.


#include "MassProcessorBlueprintRegistrySubsystem.h"

#include "MassGameplayScriptSettings.h"
#include "BlueprintClass/MassProcessorBlueprint.h"
#include "MassSimulationSubsystem.h"

void UMassProcessorBlueprintRegistrySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UMassSimulationSubsystem* SimulationSubsystem = Collection.InitializeDependency<UMassSimulationSubsystem>();
	const auto& ProcessorClasses = GetDefault<UMassGameplayScriptSettings>()->BlueprintProcessorClasses;
	for(const auto& ProcessorClass:ProcessorClasses)
	{
		if(const auto Class = ProcessorClass.TryLoadClass<UMassProcessorBlueprint>())
		{
			Class->GetDefaultObject<UMassProcessorBlueprint>()->ConfigureQueries();
			SimulationSubsystem->RegisterDynamicProcessor(*Class->GetDefaultObject<UMassProcessorBlueprint>());
		}
	}
	Super::Initialize(Collection);
}
