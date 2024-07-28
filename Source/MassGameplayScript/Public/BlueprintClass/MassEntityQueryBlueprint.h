// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityQuery.h"
#include "MassProcessorExecWrapper.h"
#include "UObject/Object.h"
#include "MassNoExprotTypes/MassNoExportTypes.h"
#include "MassEntityQueryBlueprint.generated.h"


enum class EMassFragmentAccess : uint8;
#if !CPP   //noexport class
USTRUCT(noexport, BlueprintType)
struct FMassStructSelect
{
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UScriptStruct> StructType;
};

USTRUCT(noexport, BlueprintType)
struct FMassFragmentRequirementDescription
{
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MetaStruct = "/Script/MassEntity.MassFragment"))
	FMassStructSelect StructType;
	UPROPERTY(EditAnywhere)
	EMassFragmentAccess AccessMode = EMassFragmentAccess::ReadOnly;
	UPROPERTY(EditAnywhere)
	EMassFragmentPresence Presence = EMassFragmentPresence::All;
};

USTRUCT(noexport, BlueprintType)
struct FMassTagRequirementDescription
{
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MetaStruct = "/Script/MassEntity.MassTag"))
	FMassStructSelect StructType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMassFragmentPresence Presence = EMassFragmentPresence::All;
};
#endif

struct FMassTagRequirementDescription
{
	TObjectPtr<UScriptStruct> StructType;
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
class MASSGAMEPLAYSCRIPT_API UMassEntityQueryBlueprintTransaction : public UObject
{
	GENERATED_BODY()

public:
	// UFUNCTION(BlueprintCallable)
	// void AddRequirement(
	// 	const UScriptStruct* FragmentType,
	// 	const EMassFragmentAccess AccessMode,
	// 	const EMassFragmentPresence Presence = EMassFragmentPresence::All);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMassTagRequirementDescription> TagRequirements;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMassFragmentRequirementDescription> FragmentRequirements;
	
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