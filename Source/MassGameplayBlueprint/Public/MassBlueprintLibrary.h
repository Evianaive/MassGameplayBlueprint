// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessorExecWrapper.h"
#include "MassBlueprintLibrary.generated.h"


class UMassEntityConfigAsset;
USTRUCT(BlueprintType)
struct FArrayViewBlueprint
{
	GENERATED_BODY()

	TArrayView<uint8> ArrayView;
	UPROPERTY()
	UScriptStruct* Struct;
	UPROPERTY()
	uint16 SizeOfStruct;
};

class UMassEntityQueryBlueprint;
/**
 * 
 */
UCLASS()
class MASSGAMEPLAYBLUEPRINT_API UMassBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	// UFUNCTION(BlueprintCallable)
	// static void ForEachEntityChunk(UMassEntityQueryBlueprint* InQueryBlueprint, const FMassProcessorExecWrapper& InExecWrapper, FExecuteOnChunk Function);

	UFUNCTION(BlueprintCallable)
	static int32 GetNumEntities(const FMassProcessorExecWrapper& Wrapper);

	UFUNCTION(BlueprintCallable)
	static bool GetMutableFragmentView(const FMassProcessorExecWrapper& Wrapper, UScriptStruct* Struct, FArrayViewBlueprint& OutArrayView);

	UFUNCTION(BlueprintCallable, CustomThunk, meta=(CustomStructureParam = "OutStruct"))
	static bool GetStructRef(const FArrayViewBlueprint& OutArrayView, int32 Index, int32& OutStruct);
	DECLARE_FUNCTION(execGetStructRef);

	UFUNCTION(BlueprintCallable)
	static bool SpawnEntities(const UMassEntityConfigAsset* ConfigAsset, int32 NumberToSpawn, UObject* WorldContext);
	
};
