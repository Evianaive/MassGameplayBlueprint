// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RegisterBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS(Deprecated)
class MASSGAMEPLAYSCRIPT_API UDEPRECATED_RegisterBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	static void RegisterDynamicProcessor(const TSubclassOf<UMassScriptProcessor> BP_Processor);

	UFUNCTION(BlueprintCallable)
	static void GetDerivedClass(const UClass* Class, TArray<UClass*>& SubClasses);

	UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "Class", DynamicOutputParam = "OutObject"))
	static void GetDefaultObjectBP(const UClass* Class, UObject*& OutObject);
};
