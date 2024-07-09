// Fill out your copyright notice in the Description page of Project Settings.


#include "MassProcessorBlueprint.h"

#include "MassEntityEditorSubsystem.h"
#include "MassProcessorExecWrapper.h"

UMassProcessorBlueprint::UMassProcessorBlueprint()
{
	bAutoRegisterWithProcessingPhases = false;
	bCanShowUpInSettings = false;
}

void UMassProcessorBlueprint::PostCDOContruct()
{
	Super::PostCDOContruct();
	// GetMutableDefault<UMassBlueprintSettings>()->BlueprintProcessorClasses.AddUnique(this->GetClass());
	// GetMutableDefault<UMass>()
}

void UMassProcessorBlueprint::ConfigureQueries()
{
	// const TGuardValue ScriptExecutionGuard(GAllowActorScriptExecutionInEditor, true);
	FEditorScriptExecutionGuard AllowConstructionScript;
	// Todo 可能需要放到postload里？但PostInitialize里应该已经有Script内容了
	ConfigureQueriesBP();
}

void UMassProcessorBlueprint::ConfigureQueriesBP_Implementation()
{
}

void UMassProcessorBlueprint::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	ExecuteBP(FMassProcessorExecWrapper{EntityManager,Context});
}

void UMassProcessorBlueprint::PostLoad()
{
	Super::PostLoad();
	if (GetClass()->HasAnyClassFlags(CLASS_Abstract) == false)
	{
		// ConfigureQueries();
		// check(GEditor);
		// UMassEntityEditorSubsystem* Mass = GEditor->GetEditorSubsystem<UMassEntityEditorSubsystem>();
		// check(Mass);
		// Mass->RegisterDynamicProcessor(*this);
		// bool bNeedsGameThread = false;
		// for (FMassEntityQuery* QueryPtr : OwnedQueries)
		// {
		// 	CA_ASSUME(QueryPtr);
		// 	bNeedsGameThread = (bNeedsGameThread || QueryPtr->DoesRequireGameThreadExecution());
		// }
		//
		// UE_CLOG(bRequiresGameThreadExecution != bNeedsGameThread, LogMass, Verbose
		// 	, TEXT("%s is marked bRequiresGameThreadExecution = %s, while the registered quries' requirement indicate the opposite")
		// 	, *GetProcessorName(), bRequiresGameThreadExecution ? TEXT("TRUE") : TEXT("FALSE"));
		//
		// // better safe than sorry - if queries indicate the game thread execution is required then we marked the whole processor as such
		// bRequiresGameThreadExecution = bRequiresGameThreadExecution || bNeedsGameThread;
	}
}
