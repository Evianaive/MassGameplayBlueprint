﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Helpers/MassBlueprintClassHelper.h"
#include "MassEntitySettings.h"
#include "BlueprintClass/MassScriptProcessor.h"
#include "RegisterBlueprintLibrary.h"


void UDEPRECATED_RegisterBlueprintLibrary::RegisterDynamicProcessor(const TSubclassOf<UMassScriptProcessor> BP_Processor)
{
	GetMutableDefault<UMassEntitySettings>()->AddToActiveProcessorsList(BP_Processor);
}

void UDEPRECATED_RegisterBlueprintLibrary::GetDerivedClass(const UClass* Class, TArray<UClass*>& SubClasses)
{
	GetDerivedClasses(Class, SubClasses);	
	
	for(int i = 0;i<SubClasses.Num();i++)
	{
		const auto Src = SubClasses[i];
		if(FMassBlueprintClassHelper::IsSkeletonClass(Src))
		{
			SubClasses.RemoveSwap(Src);
			i--;
		}
	}
}

void UDEPRECATED_RegisterBlueprintLibrary::GetDefaultObjectBP(const UClass* Class, UObject*& OutObject)
{
	OutObject = Class->GetDefaultObject();
}
