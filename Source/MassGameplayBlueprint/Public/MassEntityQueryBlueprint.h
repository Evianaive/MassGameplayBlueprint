// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityQuery.h"
#include "MassProcessorExecWrapper.h"
#include "UObject/Object.h"
#include "MassNoExprotTypes/MassNoExportTypes.h"
#include "MassEntityQueryBlueprint.generated.h"


#if !CPP   //noexport class
USTRUCT(noexport, BlueprintType, BlueprintInternalUseOnly)
struct FMassFragmentRequirementDescription
{
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MetaStruct = "/Script/MassEntity.MassFragment"))
	const UScriptStruct* StructType;
	UPROPERTY(EditAnywhere)
	EMassFragmentAccess AccessMode = EMassFragmentAccess::ReadOnly;
	UPROPERTY(EditAnywhere)
	EMassFragmentPresence Presence = EMassFragmentPresence::All;
};

#endif
/**
 * 
 */
/* Same Struct in Engine Plugin is FMassFragmentRequirementDescription, we can also export it in MassNoExportTypes.h*/
USTRUCT(BlueprintType)
struct FMassRequirementWrapper_Fragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MetaStruct = "/Script/MassEntity.MassFragment"))
	TObjectPtr<UScriptStruct> StructType;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMassFragmentAccess AccessMode = EMassFragmentAccess::ReadOnly;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMassFragmentPresence Presence = EMassFragmentPresence::All;
};

USTRUCT(BlueprintType)
struct FMassRequirementWrapper_Tag
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MetaStruct = "/Script/MassEntity.MassTag"))
	TObjectPtr<UScriptStruct> StructType;
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMassRequirementWrapper_Tag> TagRequirements;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMassRequirementWrapper_Fragment> FragmentRequirements;
	
	FMassEntityQuery* QueryRef;

	virtual void PreEditChange(FEditPropertyChain& PropertyAboutToChange) override;
	virtual bool CanEditChange(const FEditPropertyChain& PropertyChain) const override;
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;

	UFUNCTION(BlueprintCallable)
	static void RegisterQueryWithProcessor(UMassProcessor* InProcessor, UPARAM(ref) FMassEntityQueryBlueprint& QueryBlueprint);

	UFUNCTION(BlueprintCallable)
	void ForEachEntityChunk(const FMassProcessorExecWrapper& InExecWrapper, FExecuteOnChunk Function);
	
	// thread_local static FMassFragmentRequirementDescription PreEditRequirement; 
};