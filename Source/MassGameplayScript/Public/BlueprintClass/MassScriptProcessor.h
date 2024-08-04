// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "MassScriptProcessor.generated.h"

/**
 * 
 */

struct FMassExecutionContextWrapper;
UCLASS(Blueprintable,Abstract)
class MASSGAMEPLAYSCRIPT_API UMassScriptProcessor : public UMassProcessor
{
	GENERATED_BODY()
public:
	UMassScriptProcessor();
	virtual void PostCDOContruct() override;
	
	virtual void ConfigureQueries() override;
	UFUNCTION(BlueprintNativeEvent)
	void ConfigureQueriesBP();
	
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
	UFUNCTION(BlueprintImplementableEvent)
	void ExecuteBP(const FMassExecutionContextWrapper& ExecWrapper);
	
	virtual void PostLoad() override;
	
	UFUNCTION(BlueprintCallable)
	void RegisterQueryWithProcessor(UPARAM(ref) FMassScriptEntityQuery& QueryBlueprint);
};
