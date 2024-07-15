// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityQuery.h"
#include "MassProcessorExecWrapper.h"
#include "UObject/Object.h"
#include "MassEntityQueryBlueprint.generated.h"

/**
 * 
 */
// Todo Same Struct in Engine Plugin is FMassFragmentRequirementDescription
USTRUCT(BlueprintType)
struct FMassRequirementComponentWrapper
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MetaStruct = "/Script/MassEntity.MassFragment"))
	TObjectPtr<UScriptStruct> Struct;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMassFragmentAccess Access = EMassFragmentAccess::ReadOnly;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMassFragmentPresence Presence = EMassFragmentPresence::All;
};

class UMassEntityQueryBlueprintTransaction;
USTRUCT(BlueprintType)
struct FMassEntityQueryBlueprint
{
	GENERATED_BODY()

	// Mark: MassEntityQuery must be contained in memory of MassProcessor 
	UPROPERTY()
	FMassEntityQuery QueryInternal;
	// UPROPERTY(EditAnywhere,Instanced,BlueprintReadWrite,meta=(EditInline))
	UPROPERTY(EditAnywhere,Instanced,BlueprintReadWrite)
	TObjectPtr<UMassEntityQueryBlueprintTransaction> Transaction;
};

// DefaultToInstanced
UCLASS(BlueprintType, editinlinenew, Within = MassProcessorBlueprint)
class MASSGAMEPLAYBLUEPRINT_API UMassEntityQueryBlueprintTransaction : public UObject
{
	GENERATED_BODY()

public:
	// UFUNCTION(BlueprintCallable)
	// void AddRequirement(
	// 	const UScriptStruct* FragmentType,
	// 	const EMassFragmentAccess AccessMode,
	// 	const EMassFragmentPresence Presence = EMassFragmentPresence::All);
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// TArray<FMassRequirementComponentWrapper> TagRequirements; //Todo Add Wrapper Type

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMassRequirementComponentWrapper> FragmentRequirements;
	
	FMassEntityQuery* QueryRef;
	
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;

	UFUNCTION(BlueprintCallable)
	static void RegisterQueryWithProcessor(UMassProcessor* InProcessor, UPARAM(ref) FMassEntityQueryBlueprint& QueryBlueprint);

	UFUNCTION(BlueprintCallable)
	void ForEachEntityChunk(const FMassProcessorExecWrapper& InExecWrapper, FExecuteOnChunk Function);

};