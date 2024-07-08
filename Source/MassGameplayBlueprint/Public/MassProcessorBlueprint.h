// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "MassProcessorBlueprint.generated.h"

/**
 * 
 */

struct FMassProcessorExecWrapper;
UCLASS(Blueprintable,Abstract)
class MASSGAMEPLAYBLUEPRINT_API UMassProcessorBlueprint : public UMassProcessor
{
	GENERATED_BODY()
public:
// Todo don't auto register!!
	virtual void PostCDOContruct() override;
	
	virtual void ConfigureQueries() override;
	UFUNCTION(BlueprintNativeEvent)
	void ConfigureQueriesBP();
	// UFUNCTION(BlueprintCallable)
	// void AddQuery();
	
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
	UFUNCTION(BlueprintImplementableEvent)
	void ExecuteBP(const FMassProcessorExecWrapper& ExecWrapper);

	// UPROPERTY(EditAnywhere,Instanced,EditInline)
	
	virtual void PostLoad() override;
};
