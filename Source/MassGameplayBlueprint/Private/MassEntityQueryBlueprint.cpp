// Fill out your copyright notice in the Description page of Project Settings.


#include "MassEntityQueryBlueprint.h"

#include "MassProcessor.h"


// void UMassEntityQueryBlueprintTransaction::AddRequirement(const UScriptStruct* FragmentType,
// 	const EMassFragmentAccess AccessMode, const EMassFragmentPresence Presence)
// {
// 	QueryInternal.AddRequirement(FragmentType,AccessMode,Presence);
// }

void UMassEntityQueryBlueprintTransaction::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	static FName StructName = GET_MEMBER_NAME_CHECKED(FMassRequirementComponentWrapper,Struct);
	static FName RequiresName = GET_MEMBER_NAME_CHECKED(UMassEntityQueryBlueprintTransaction,FragmentRequirements);
	if( StructName == PropertyChangedEvent.Property->GetFName())
	{
		// Todo Add Already Exist Type Check
		const int32 RequireArrayIndex = PropertyChangedEvent.GetArrayIndex(RequiresName.ToString());
		if(!FragmentRequirements[RequireArrayIndex].Struct->IsChildOf(FMassFragment::StaticStruct()))
		{
			FragmentRequirements[RequireArrayIndex].Struct = nullptr;
		}
	}
	UObject::PostEditChangeChainProperty(PropertyChangedEvent);
}

void UMassEntityQueryBlueprintTransaction::RegisterQueryWithProcessor(UMassProcessor* InProcessor,  FMassEntityQueryBlueprint& QueryBlueprint)
{
	if(InProcessor==nullptr)
		InProcessor = Cast<UMassProcessor>(QueryBlueprint.Transaction->GetOuter());
	for (const auto& FragmentRequirement : QueryBlueprint.Transaction->FragmentRequirements)
	{
		QueryBlueprint.QueryInternal.AddRequirement(
			FragmentRequirement.Struct,
			FragmentRequirement.Access,
			FragmentRequirement.Presence);	
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
