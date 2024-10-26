// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintClass/MassExecutionContextWrapper.h"
#include "MassBlueprintLibrary.generated.h"


struct FMassScriptEntityQuery;
class UMassProcessor;
class UMassEntityConfigAsset;

USTRUCT(BlueprintType)
struct FEmptyStubStruct
{
	GENERATED_BODY()
	uint8 StubProp;
};

USTRUCT(BlueprintType)
struct FScriptArrayView
{
	GENERATED_BODY()

#if CPP
	FScriptArrayView()
		: ArrayView()
		, ArrayMax(0)		
		, Struct(nullptr)
		, SizeOfStruct(0)
	{		
	};
	
	FScriptArrayView& operator = (const FScriptArrayView&);
	explicit FScriptArrayView(const TArrayView<uint8>& InArrayView, const UScriptStruct* InStruct);
	~FScriptArrayView();

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
class MASSGAMEPLAYSCRIPT_API UMassBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// UFUNCTION(BlueprintCallable)
	// static void ForEachEntityChunk(UMassEntityQueryBlueprint* InQueryBlueprint, const FMassProcessorExecWrapper& InExecWrapper, FExecuteOnChunk Function);

	UFUNCTION(BlueprintCallable)
	static int32 GetNumEntities(const FMassExecutionContextWrapper& Wrapper);

	UFUNCTION(BlueprintCallable)
	static bool GetMutableFragmentView(const FMassExecutionContextWrapper& Wrapper, const UScriptStruct* Struct, FScriptArrayView& OutArrayView);

	UFUNCTION(BlueprintPure, BlueprintCallable, CustomThunk, meta=(CustomStructureParam = "OutStruct"))
	static bool GetStructRef(UPARAM(ref) const FScriptArrayView& ArrayView, int32 Index, int32& OutStruct);
	DECLARE_FUNCTION(execGetStructRef);

	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContext"))
	static bool SpawnEntities(const UMassEntityConfigAsset* ConfigAsset, int32 NumberToSpawn, UObject* WorldContext);

	UFUNCTION(BlueprintCallable, CustomThunk, meta=(ArrayParm = "OutArray"))
	static bool GetArrayFromView(TArray<int32>& OutArray, UPARAM(ref) const FScriptArrayView& OutArrayView);
	DECLARE_FUNCTION(execGetArrayFromView);

	UFUNCTION(BlueprintCallable)
	static void ForEachEntityChunk(UPARAM(ref)FMassScriptEntityQuery& QueryWrapper, const FMassExecutionContextWrapper& InExecWrapper, FExecuteOnChunk Function);
};
