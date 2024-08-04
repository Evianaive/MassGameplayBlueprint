// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintClass/MassScriptEntityQuery.h"
#include "Utility/PrivateAccessor.hpp"

DECLARE_PRIVATE_ACCESS(FMassEntityQuery,ArchetypeFragmentMapping,TArray<FMassQueryRequirementIndicesMapping>)

FMassFragmentRequirements& FMassScriptEntityQuery::AddChunkRequirement(
	const FMassChunkFragmentRequirementDescription& ChunkDescription)
{
	if(!ChunkDescription.StructType)
		return *this;		
	ensureMsgf(ChunkDescription.StructType->IsChildOf(FMassChunkFragment::StaticStruct()),
		TEXT("Given struct doesn't represent a valid chunk fragment type. Make sure to inherit from FMassChunkFragment or one of its child-types."));
	checkf(ChunkFragmentRequirements.FindByPredicate([&](const FMassFragmentRequirementDescription& Item) { return Item.StructType == ChunkDescription.StructType; }) == nullptr
		, TEXT("Duplicated requirements are not supported. %s already present"), *ChunkDescription.StructType->GetName());
	checkfSlow(Presence != EMassFragmentPresence::Any, TEXT("\'Any\' is not a valid Presence value for AddChunkRequirement."));

	switch (ChunkDescription.Presence)
	{
	case EMassFragmentPresence::All:
		RequiredAllChunkFragments.Add(*ChunkDescription.StructType);
		ChunkFragmentRequirements.Emplace(ChunkDescription);
		break;
	case EMassFragmentPresence::Optional:
		RequiredOptionalChunkFragments.Add(*ChunkDescription.StructType);
		ChunkFragmentRequirements.Emplace(ChunkDescription);
		break;
	case EMassFragmentPresence::None:
		RequiredNoneChunkFragments.Add(*ChunkDescription.StructType);
		break;
	}
	IncrementChangeCounter();
	return *this;
}

FMassFragmentRequirements& FMassScriptEntityQuery::AddConstSharedRequirement(
	const FMassConstSharedFragmentRequirementDescription& ConstSharedDescription)
{
	if(!ConstSharedDescription.StructType)
		return *this;
	ensureMsgf(ConstSharedDescription.StructType->IsChildOf(FMassChunkFragment::StaticStruct()),
		TEXT("Given struct doesn't represent a valid shared fragment type. Make sure to inherit from FMassSharedFragment or one of its child-types."));
	checkf(ConstSharedFragmentRequirements.FindByPredicate([&](const FMassFragmentRequirementDescription& Item) { return Item.StructType == ConstSharedDescription.StructType; }) == nullptr
		, TEXT("Duplicated requirements are not supported. %s already present"), *ConstSharedDescription.StructType->GetName());
	checkfSlow(Presence != EMassFragmentPresence::Any, TEXT("\'Any\' is not a valid Presence value for AddSharedRequirement."));

	switch (ConstSharedDescription.Presence)
	{
	case EMassFragmentPresence::All:
		RequiredAllSharedFragments.Add(*ConstSharedDescription.StructType);
		ConstSharedFragmentRequirements.Emplace(ConstSharedDescription.StructType, EMassFragmentAccess::ReadOnly, ConstSharedDescription.Presence);
		break;
	case EMassFragmentPresence::Optional:
		RequiredOptionalSharedFragments.Add(*ConstSharedDescription.StructType);
		ConstSharedFragmentRequirements.Emplace(ConstSharedDescription.StructType, EMassFragmentAccess::ReadOnly, ConstSharedDescription.Presence);
		break;
	case EMassFragmentPresence::None:
		RequiredNoneSharedFragments.Add(*ConstSharedDescription.StructType);
		break;
	}
	IncrementChangeCounter();
	return *this;
}

FMassFragmentRequirements& FMassScriptEntityQuery::AddSharedRequirement(
	const FMassSharedFragmentRequirementDescription& SharedDescription)

{
	if(!SharedDescription.StructType)
		return *this;
	ensureMsgf(SharedDescription.StructType->IsChildOf(FMassChunkFragment::StaticStruct()),
		TEXT("Given struct doesn't represent a valid shared fragment type. Make sure to inherit from FMassSharedFragment or one of its child-types."));
		
	checkf(SharedFragmentRequirements.FindByPredicate([&](const FMassFragmentRequirementDescription& Item) { return Item.StructType == SharedDescription.StructType; }) == nullptr
		, TEXT("Duplicated requirements are not supported. %s already present"), *SharedDescription.StructType->GetName());
	checkfSlow(Presence != EMassFragmentPresence::Any, TEXT("\'Any\' is not a valid Presence value for AddSharedRequirement."));

	switch (SharedDescription.Presence)
	{
	case EMassFragmentPresence::All:
		RequiredAllSharedFragments.Add(*SharedDescription.StructType);
		SharedFragmentRequirements.Emplace(SharedDescription);
		if (SharedDescription.AccessMode == EMassFragmentAccess::ReadWrite)
		{
			// bRequiresGameThreadExecution |= TMassSharedFragmentTraits<T>::GameThreadOnly;
			uint8& BitField = *reinterpret_cast<uint8*>(&(this->*PRIVATE_ACCESS(FMassEntityQuery,ArchetypeFragmentMapping))[1]);
			BitField |= 0x01;
		}
		break;
	case EMassFragmentPresence::Optional:
		RequiredOptionalSharedFragments.Add(*SharedDescription.StructType);
		SharedFragmentRequirements.Emplace(SharedDescription);
		if (SharedDescription.AccessMode == EMassFragmentAccess::ReadWrite)
		{
			// bRequiresGameThreadExecution |= TMassSharedFragmentTraits<T>::GameThreadOnly;
			uint8& BitField = *reinterpret_cast<uint8*>(&(this->*PRIVATE_ACCESS(FMassEntityQuery,ArchetypeFragmentMapping))[1]);
			BitField |= 0x01;
		}
		break;
	case EMassFragmentPresence::None:
		RequiredNoneSharedFragments.Add(*SharedDescription.StructType);
		break;
	}
	IncrementChangeCounter();
	return *this;
}	
