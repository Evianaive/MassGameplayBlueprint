// Fill out your copyright notice in the Description page of Project Settings.


#include "MassBlueprintSettings.h"

#include "Helpers/MassBlueprintClassHelper.h"
#include "MassEntitySettings.h"
#include "BlueprintClass/MassProcessorBlueprint.h"

UMassBlueprintSettings::UMassBlueprintSettings(const FObjectInitializer& ObjectInitializer)
{
	FCoreDelegates::OnPostEngineInit.AddUObject(this, &UMassBlueprintSettings::OnPostEngineInit);
}

void UMassBlueprintSettings::AddBlueprintProcessorClass(TSubclassOf<UMassProcessorBlueprint> Class)
{
	if(FMassBlueprintClassHelper::IsSkeletonClass(Class))
		return;
	BlueprintProcessorClasses.AddUnique(Class->GetClassPathName().ToString());
}

void UMassBlueprintSettings::RemoveBlueprintProcessorAsset(const FSoftClassPath& ClassPath)
{
	BlueprintProcessorClasses.Remove(ClassPath);
}

void UMassBlueprintSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	OnSettingsChange.Broadcast(PropertyChangedEvent);
}

void UMassBlueprintSettings::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
}

void UMassBlueprintSettings::BeginDestroy()
{
	Super::BeginDestroy();
}

void UMassBlueprintSettings::OnPostEngineInit()
{
	// bEngineInitialized = true;
	// for(auto Class : BlueprintProcessorClasses)
	// {
	// 	if(Class)
	// 		GetMutableDefault<UMassEntitySettings>()->AddToActiveProcessorsList(Class);
	// }
}
