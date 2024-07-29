﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Helpers/MassBlueprintLibrary.h"

#include "MassEntityConfigAsset.h"
#include "BlueprintClass/MassEntityQueryWrapper.h"
#include "MassExecutionContext.h"
#include "MassSpawnerSubsystem.h"
#include "Blueprint/BlueprintExceptionInfo.h"

#define LOCTEXT_NAMESPACE "MassBlueprint"

FArrayViewBlueprint& FArrayViewBlueprint::operator=(const FArrayViewBlueprint& InArrayViewBlueprint)
{
	ArrayView = InArrayViewBlueprint.ArrayView;
	ArrayMax = InArrayViewBlueprint.ArrayMax;
	Struct = InArrayViewBlueprint.Struct;
	SizeOfStruct = 0;
	return *this;
}

FArrayViewBlueprint::FArrayViewBlueprint(
	const TArrayView<uint8>& InArrayView,
	const UScriptStruct* InStruct)
	: ArrayView(InArrayView)
	, Struct(InStruct)
	, SizeOfStruct(0)
{
}

FArrayViewBlueprint::~FArrayViewBlueprint()
{
	ArrayView.~TArrayView();
}

int32 UMassBlueprintLibrary::GetNumEntities(const FMassExecutionContextWrapper& Wrapper)
{
	return Wrapper.Context->GetNumEntities();
}

bool UMassBlueprintLibrary::GetMutableFragmentView(const FMassExecutionContextWrapper& Wrapper, const UScriptStruct* Struct, FArrayViewBlueprint& OutArrayView)
{
	auto FragmentView = Wrapper.Context->GetMutableFragmentView(Struct);

	if(!Wrapper.Context->DoesArchetypeHaveFragment(*Struct))
	{
		// Todo Add private access to check view exist, otherwise return false
		// Todo Currently we don't know if this equals to Context->FragmentViews->Find is nullptr
		return false;
	}
	
	// auto DataBlockStart = FragmentView.begin();
	OutArrayView.ArrayView = reinterpret_cast<TArrayView<uint8>&>(FragmentView);
	OutArrayView.ArrayMax = FragmentView.Num();
	OutArrayView.Struct = Struct;
	OutArrayView.SizeOfStruct = Struct->GetStructureSize();

	return true;
}

bool UMassBlueprintLibrary::GetStructRef(const FArrayViewBlueprint& ArrayView, int32 Index, int32& OutStruct)
{
	return false;
}
DEFINE_FUNCTION(UMassBlueprintLibrary::execGetStructRef)
{
	P_GET_STRUCT_REF(FArrayViewBlueprint, ArrayView);
	P_GET_PROPERTY(FIntProperty,Index);
	
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentPropertyContainer = nullptr;
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	
	void* PropertyAddress = Stack.MostRecentPropertyAddress;
	FStructProperty* ValueProp = CastField<FStructProperty>(Stack.MostRecentProperty);

	P_FINISH
	
	bool bResult = true;
	
	if (!PropertyAddress || !ValueProp)
	{
		FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AbortExecution,
			LOCTEXT("GetStructRef", "InValid Output Type")
		);

		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		bResult = false;
	}
	if(ValueProp->Struct != ArrayView.Struct)
	{
		FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AbortExecution,
			LOCTEXT("GetStructRef", "Unmatched Output Type")
		);

		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		bResult = false;
	}

	// Todo there is still a copy, how can I return the reference to Data Directly?
	if(bResult)
	{
		P_NATIVE_BEGIN
		// Stack.MostRecentPropertyAddress = ArrayView.ArrayView.GetData()+ArrayView.SizeOfStruct*Index;
		ArrayView.Struct->CopyScriptStruct(
			PropertyAddress,
			Stack.MostRecentPropertyAddress,
			1);
		P_NATIVE_END
	}
	*(bool*)RESULT_PARAM = bResult;
}

bool UMassBlueprintLibrary::SpawnEntities(const UMassEntityConfigAsset* ConfigAsset, int32 NumberToSpawn, UObject* WorldContext)
{
	// Todo How to get world for Blueprint static Function?
	const auto World = WorldContext->GetWorld();
	if(!World)
		return false;
	UE_LOG(LogTemp,Log,TEXT("%s"),*World->GetName());
	auto& Template = ConfigAsset->GetOrCreateEntityTemplate(*World);
	TArray<FMassEntityHandle> Handles;
	World->GetSubsystem<UMassSpawnerSubsystem>()->SpawnEntities(Template,NumberToSpawn,Handles);
	return true;
// Todo WorldContext
// 	void UKismetSystemLibrary::PrintString(const UObject* WorldContextObject, const FString& InString, bool bPrintToScreen, bool bPrintToLog, FLinearColor TextColor, float Duration, const FName Key)
// 	{
// #if !(UE_BUILD_SHIPPING || UE_BUILD_TEST) || USE_LOGGING_IN_SHIPPING // Do not Print in Shipping or Test unless explictly enabled.
//
// 		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
// 		FString Prefix;
// 		if (World)
// 		{
// 			if (World->WorldType == EWorldType::PIE)
// 			{
// 				switch(World->GetNetMode())
// 				{
// 				case NM_Client:
// 					// GPlayInEditorID 0 is always the server, so 1 will be first client.
// 					// You want to keep this logic in sync with GeneratePIEViewportWindowTitle and UpdatePlayInEditorWorldDebugString
// 					Prefix = FString::Printf(TEXT("Client %d: "), GPlayInEditorID);
// 					break;
// 				case NM_DedicatedServer:
// 				case NM_ListenServer:
// 					Prefix = FString::Printf(TEXT("Server: "));
// 					break;
// 				case NM_Standalone:
// 					break;
// 				}
// 			}
// 		}
// 	}

}

bool UMassBlueprintLibrary::GetArrayFromView(TArray<int32>& OutArray, const FArrayViewBlueprint& ArrayView)
{
	return false;
}
DEFINE_FUNCTION(UMassBlueprintLibrary::execGetArrayFromView)
{
	Stack.MostRecentProperty = nullptr;
	Stack.StepCompiledIn<FArrayProperty>(NULL);
	// Stack.MostRecentPropertyAddress;
	FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Stack.MostRecentProperty);
	if (!ArrayProperty)
	{
		Stack.bArrayContextFailed = true;
		return;
	}
	const UScriptStruct* InnerStruct = nullptr;
	if(auto* StructProperty = CastField<FStructProperty>(ArrayProperty->Inner))
	{
		InnerStruct = StructProperty->Struct;
	}
	P_GET_STRUCT_REF(FArrayViewBlueprint,ArrayView);

	P_FINISH;
	P_NATIVE_BEGIN;
	if(ArrayView.Struct == InnerStruct)
	{
		*(bool*)RESULT_PARAM = true;
		Stack.MostRecentPropertyAddress = (uint8*)&ArrayView.GetAsArray();
	}
	P_NATIVE_END;
}

void UMassBlueprintLibrary::RegisterQueryWithProcessor(UMassProcessor* InProcessor,
	FMassEntityQueryWrapper& QueryBlueprint)
{
	if(InProcessor==nullptr)
	{
		// Todo Waring by dialog
		return;
	}	
	QueryBlueprint.QueryInternal.Clear();
	for (const auto& FragmentRequirement : QueryBlueprint.Transaction.FragmentRequirements)
	{
		QueryBlueprint.QueryInternal.AddRequirement(
			FragmentRequirement.StructType,
			FragmentRequirement.AccessMode,
			FragmentRequirement.Presence);
	}
	for (const auto& TagRequirement : QueryBlueprint.Transaction.TagRequirements)
	{
		QueryBlueprint.QueryInternal.AddTagRequirement(
			*TagRequirement.StructType,
			TagRequirement.Presence);
	}
	if(!InProcessor)
		return;
	QueryBlueprint.QueryInternal.RegisterWithProcessor(*InProcessor);
}

void UMassBlueprintLibrary::ForEachEntityChunk(FMassEntityQueryWrapper& QueryWrapper,
	const FMassExecutionContextWrapper& InExecWrapper, FExecuteOnChunk Function)
{
	QueryWrapper.QueryInternal.ForEachEntityChunk(
		*InExecWrapper.EntityManager,
		*InExecWrapper.Context,
		[&](FMassExecutionContext& Context)
		{
			Function.Execute(FMassExecutionContextWrapper{*InExecWrapper.EntityManager,Context});
		}
	);
}

#undef LOCTEXT_NAMESPACE
