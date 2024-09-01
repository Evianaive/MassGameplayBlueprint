// Fill out your copyright notice in the Description page of Project Settings.


#include "MassGameplayScriptSettings.h"

#include "Helpers/MassBlueprintClassHelper.h"
#include "MassEntitySettings.h"
#include "BlueprintClass/MassScriptProcessor.h"

UMassGameplayScriptSettings::UMassGameplayScriptSettings(const FObjectInitializer& ObjectInitializer)
{
	FCoreDelegates::OnPostEngineInit.AddUObject(this, &UMassGameplayScriptSettings::OnPostEngineInit);
}

void UMassGameplayScriptSettings::AddScriptProcessorClass(TSubclassOf<UMassScriptProcessor> Class)
{
	if(FMassBlueprintClassHelper::IsSkeletonClass(Class))
		return;
	ScriptProcessorClasses.AddUnique(Class->GetClassPathName().ToString());
}

void UMassGameplayScriptSettings::RemoveScriptProcessorAsset(const FSoftClassPath& ClassPath)
{
	ScriptProcessorClasses.Remove(ClassPath);
}

void UMassGameplayScriptSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	OnSettingsChange.Broadcast(PropertyChangedEvent);
}

void UMassGameplayScriptSettings::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
}

void UMassGameplayScriptSettings::BeginDestroy()
{
	Super::BeginDestroy();
}

void UMassGameplayScriptSettings::OnPostEngineInit()
{
	// bEngineInitialized = true;
	// for(auto Class : BlueprintProcessorClasses)
	// {
	// 	if(Class)
	// 		GetMutableDefault<UMassEntitySettings>()->AddToActiveProcessorsList(Class);
	// }
}
