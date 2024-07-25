// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintClass/MassEntityQueryBlueprint.h"

#include "MassProcessor.h"


// void UMassEntityQueryBlueprintTransaction::AddRequirement(const UScriptStruct* FragmentType,
// 	const EMassFragmentAccess AccessMode, const EMassFragmentPresence Presence)
// {
// 	QueryInternal.AddRequirement(FragmentType,AccessMode,Presence);
// }

void UMassEntityQueryBlueprintTransaction::PreEditChange(FEditPropertyChain& PropertyAboutToChange)
{
	UObject::PreEditChange(PropertyAboutToChange);
}

bool UMassEntityQueryBlueprintTransaction::CanEditChange(const FEditPropertyChain& PropertyChain) const
{
	return UObject::CanEditChange(PropertyChain);
}


void UMassEntityQueryBlueprintTransaction::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	static FName StructName = GET_MEMBER_NAME_CHECKED(FMassRequirementWrapper_Fragment,StructType);
	if( StructName == PropertyChangedEvent.Property->GetFName())
	{
		static FName FragmentRequireName = GET_MEMBER_NAME_CHECKED(UMassEntityQueryBlueprintTransaction,FragmentRequirements);
		static FName TagRequireName = GET_MEMBER_NAME_CHECKED(UMassEntityQueryBlueprintTransaction,TagRequirements);

		auto CheckValid = [&](auto& Requirements, UScriptStruct* CheckStruct, const FName& Name)
		{
			const int32 RequireArrayIndex = PropertyChangedEvent.GetArrayIndex(Name.ToString());
			if(RequireArrayIndex == -1)
				return false;			
			if(!Requirements[RequireArrayIndex].StructType->IsChildOf(CheckStruct))
			{
				// Todo Set to Old Data If Failed
				Requirements[RequireArrayIndex].StructType = nullptr;
			}
			return true;			
		};
		if(CheckValid(FragmentRequirements,FMassFragment::StaticStruct(),FragmentRequireName))
			return;
		if(CheckValid(TagRequirements,FMassTag::StaticStruct(),TagRequireName))
			return;
	}
	UObject::PostEditChangeChainProperty(PropertyChangedEvent);
}

void UMassEntityQueryBlueprintTransaction::RegisterQueryWithProcessor(UMassProcessor* InProcessor,  FMassEntityQueryBlueprint& QueryBlueprint)
{
	if(InProcessor==nullptr)
		InProcessor = Cast<UMassProcessor>(QueryBlueprint.Transaction->GetOuter());
	
	QueryBlueprint.QueryInternal.Clear();
	for (const auto& FragmentRequirement : QueryBlueprint.Transaction->FragmentRequirements)
	{
		QueryBlueprint.QueryInternal.AddRequirement(
			FragmentRequirement.StructType,
			FragmentRequirement.AccessMode,
			FragmentRequirement.Presence);	
	}
	for (const auto& TagRequirement : QueryBlueprint.Transaction->TagRequirements)
	{
		QueryBlueprint.QueryInternal.AddTagRequirement(
			*TagRequirement.StructType,
			TagRequirement.Presence);
	}
	if(!InProcessor)
		return;
	QueryBlueprint.Transaction->QueryRef = &QueryBlueprint.QueryInternal;
	QueryBlueprint.QueryInternal.RegisterWithProcessor(*InProcessor);
}

void UMassEntityQueryBlueprintTransaction::ForEachEntityChunk(const FMassProcessorExecWrapper& InExecWrapper, FExecuteOnChunk Function)
{
	if(!QueryRef)
		return;
	QueryRef->ForEachEntityChunk(
		*InExecWrapper.EntityManager,
		*InExecWrapper.Context,
		[&](FMassExecutionContext& Context)
		{
			Function.Execute(FMassProcessorExecWrapper{*InExecWrapper.EntityManager,Context});
		}
	);
}
