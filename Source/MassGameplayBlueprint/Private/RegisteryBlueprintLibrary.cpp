// Fill out your copyright notice in the Description page of Project Settings.


#include "MassBlueprintClassHelper.h"
#include "MassEntitySettings.h"
#include "MassProcessorBlueprint.h"
#include "RegisterBlueprintLibrary.h"


void URegisterBlueprintLibrary::RegisterDynamicProcessor(const TSubclassOf<UMassProcessorBlueprint> BP_Processor)
{
	GetMutableDefault<UMassEntitySettings>()->AddToActiveProcessorsList(BP_Processor);
}

void URegisterBlueprintLibrary::GetDerivedClass(const UClass* Class, TArray<UClass*>& SubClasses)
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

void URegisterBlueprintLibrary::GetDefaultObjectBP(const UClass* Class, UObject*& OutObject)
{
	OutObject = Class->GetDefaultObject();
}
