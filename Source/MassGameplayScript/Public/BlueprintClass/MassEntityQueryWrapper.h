// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityQuery.h"
#include "MassExecutionContextWrapper.h"
#include "UObject/Object.h"
#include "MassNoExprotTypes/MassNoExportTypes.h"
#include "MassEntityQueryWrapper.generated.h"


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

USTRUCT(BlueprintType)
struct MASSGAMEPLAYSCRIPT_API FMassEntityQueryBlueprintTransaction
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMassTagRequirementDescription> TagRequirements;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMassFragmentRequirementDescription> FragmentRequirements;
	
	// thread_local static FMassFragmentRequirementDescription PreEditRequirement; 
};

USTRUCT(BlueprintType)
struct FMassEntityQueryWrapper
{
	GENERATED_BODY()
	// Mark: MassEntityQuery must be contained in memory of MassProcessor 
	UPROPERTY()
	FMassEntityQuery QueryInternal;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FMassEntityQueryBlueprintTransaction Transaction;
};