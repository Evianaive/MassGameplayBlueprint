// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityQuery.h"
#include "MassExecutionContextWrapper.h"
#include "MassSubsystemBase.h"
#include "UObject/Object.h"
#include "MassNoExprotTypes/MassNoExportTypes.h"
#include "MassScriptEntityQuery.generated.h"


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
struct FMassChunkFragmentRequirementDescription
{
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MetaStruct = "/Script/MassEntity.MassChunkFragment"))
	FMassStructSelect StructType;
	UPROPERTY(EditAnywhere)
	EMassFragmentAccess AccessMode = EMassFragmentAccess::ReadOnly;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMassFragmentPresence Presence = EMassFragmentPresence::All;
};

USTRUCT(noexport, BlueprintType)
struct FMassSharedFragmentRequirementDescription
{
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MetaStruct = "/Script/MassEntity.MassSharedFragment"))
	FMassStructSelect StructType;
	UPROPERTY(EditAnywhere)
	EMassFragmentAccess AccessMode = EMassFragmentAccess::ReadOnly;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
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

USTRUCT(noexport, BlueprintType)
struct FMassConstSharedFragmentRequirementDescription
{
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MetaStruct = "/Script/MassEntity.MassSharedFragment"))
	FMassStructSelect StructType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMassFragmentPresence Presence = EMassFragmentPresence::All;
};

#endif

struct FMassSharedFragmentRequirementDescription : public FMassFragmentRequirementDescription
{};
struct FMassChunkFragmentRequirementDescription : public FMassFragmentRequirementDescription
{};

struct FMassNoWriteRequirementDescription
{
	TObjectPtr<UScriptStruct> StructType;
	EMassFragmentPresence Presence = EMassFragmentPresence::All;
};
struct FMassTagRequirementDescription : public FMassNoWriteRequirementDescription
{};
struct FMassConstSharedFragmentRequirementDescription : public FMassNoWriteRequirementDescription
{};

USTRUCT(BlueprintType)
struct FMassSubsystemRequirementDescription
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UMassSubsystemBase> SubSystem;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EMassFragmentAccess AccessMode = EMassFragmentAccess::ReadOnly;
};

USTRUCT(BlueprintType)
struct MASSGAMEPLAYSCRIPT_API FMassEntityQueryBlueprintTransaction
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fragment")
	TArray<FMassTagRequirementDescription> TagRequirements;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fragment")
	TArray<FMassFragmentRequirementDescription> FragmentRequirements;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fragment")
	TArray<FMassChunkFragmentRequirementDescription> ChunkFragmentRequirements;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fragment")
	TArray<FMassSharedFragmentRequirementDescription> SharedFragmentRequirements;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fragment")
	TArray<FMassConstSharedFragmentRequirementDescription> ConstSharedFragmentRequirements;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Subsystems")
	TArray<FMassSubsystemRequirementDescription> SubsystemRequirements;
	// thread_local static FMassFragmentRequirementDescription PreEditRequirement;
};

// Mark: MassEntityQuery must be contained in memory of MassProcessor 
USTRUCT(BlueprintType)
struct FMassScriptEntityQuery : public FMassEntityQuery
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FMassEntityQueryBlueprintTransaction Transaction;
	
	FMassFragmentRequirements& AddChunkRequirement(const FMassChunkFragmentRequirementDescription& ChunkDescription);
	FMassFragmentRequirements& AddConstSharedRequirement(const FMassConstSharedFragmentRequirementDescription& ConstSharedDescription);	
	FMassFragmentRequirements& AddSharedRequirement(const FMassSharedFragmentRequirementDescription& SharedDescription);
};