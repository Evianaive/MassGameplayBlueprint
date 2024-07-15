// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessorExecWrapper.h"
#include "MassBlueprintLibrary.generated.h"


class UMassEntityConfigAsset;

USTRUCT(BlueprintType)
struct FEmptyStubStruct
{
	GENERATED_BODY()
	uint8 StubProp;
};

USTRUCT(BlueprintType)
struct FArrayViewBlueprint
{
	GENERATED_BODY()

#if CPP
	FArrayViewBlueprint()
		: ArrayView()
		, ArrayMax(0)		
		, Struct(nullptr)
		, SizeOfStruct(0)
	{		
	};
	
	FArrayViewBlueprint& operator = (const FArrayViewBlueprint&);
	explicit FArrayViewBlueprint(const TArrayView<uint8>& InArrayView, const UScriptStruct* InStruct);
	~FArrayViewBlueprint();

	union
	{
		struct
		{
			TArrayView<uint8> ArrayView;
			int32 ArrayMax;
		};
#endif
	UPROPERTY(BlueprintReadWrite)
	TArray<FEmptyStubStruct> AsArray;
#if CPP			
	};
#endif	
	UPROPERTY()
	const UScriptStruct* Struct;
	UPROPERTY()
	uint16 SizeOfStruct;
			
	const decltype(AsArray)& GetAsArray() const
	{
		return AsArray;
	}
};

class UMassEntityQueryBlueprint;
/**
 * 
 */
UCLASS()
class MASSGAMEPLAYBLUEPRINT_API UMassBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// UFUNCTION(BlueprintCallable)
	// static void ForEachEntityChunk(UMassEntityQueryBlueprint* InQueryBlueprint, const FMassProcessorExecWrapper& InExecWrapper, FExecuteOnChunk Function);

	UFUNCTION(BlueprintCallable)
	static int32 GetNumEntities(const FMassProcessorExecWrapper& Wrapper);

	UFUNCTION(BlueprintCallable)
	static bool GetMutableFragmentView(const FMassProcessorExecWrapper& Wrapper, const UScriptStruct* Struct, FArrayViewBlueprint& OutArrayView);

	UFUNCTION(BlueprintPure, BlueprintCallable, CustomThunk, meta=(CustomStructureParam = "OutStruct"))
	static bool GetStructRef(UPARAM(ref) const FArrayViewBlueprint& ArrayView, int32 Index, int32& OutStruct);
	DECLARE_FUNCTION(execGetStructRef);

	UFUNCTION(BlueprintCallable)
	static bool SpawnEntities(const UMassEntityConfigAsset* ConfigAsset, int32 NumberToSpawn, UObject* WorldContext);

	UFUNCTION(BlueprintCallable, CustomThunk, meta=(ArrayParm = "OutArray"))
	static bool GetArrayFromView(TArray<int32>& OutArray, UPARAM(ref) const FArrayViewBlueprint& OutArrayView);
	DECLARE_FUNCTION(execGetArrayFromView);	
};
