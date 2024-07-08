// Fill out your copyright notice in the Description page of Project Settings.


#include "MassBlueprintLibrary.h"

#include "MassEntityConfigAsset.h"
#include "MassEntityQueryBlueprint.h"
#include "MassExecutionContext.h"
#include "MassSpawnerSubsystem.h"
#include "Blueprint/BlueprintExceptionInfo.h"

#define LOCTEXT_NAMESPACE "MassBlueprint"

int32 UMassBlueprintLibrary::GetNumEntities(const FMassProcessorExecWrapper& Wrapper)
{
	return Wrapper.Context->GetNumEntities();
}

bool UMassBlueprintLibrary::GetMutableFragmentView(const FMassProcessorExecWrapper& Wrapper, UScriptStruct* Struct, FArrayViewBlueprint& OutArrayView)
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
	OutArrayView.Struct = Struct;
	OutArrayView.SizeOfStruct = Struct->GetStructureSize();

	return true;
}

bool UMassBlueprintLibrary::GetStructRef(const FArrayViewBlueprint& OutArrayView, int32 Index, int32& OutStruct)
{
	return false;
}
DEFINE_FUNCTION(UMassBlueprintLibrary::execGetStructRef)
{
	P_GET_STRUCT_REF(FArrayViewBlueprint, OutArrayView);
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
	if(ValueProp->Struct != OutArrayView.Struct)
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
		OutArrayView.Struct->CopyScriptStruct(
			PropertyAddress,
			OutArrayView.ArrayView.GetData()+OutArrayView.SizeOfStruct*Index,
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
// Todo
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

#undef LOCTEXT_NAMESPACE
