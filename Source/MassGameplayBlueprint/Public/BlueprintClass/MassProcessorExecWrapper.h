// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "MassProcessorExecWrapper.generated.h"

struct FMassEntityManager;
struct FMassExecutionContext;
USTRUCT(BlueprintType)
struct FMassProcessorExecWrapper
{
	GENERATED_BODY()
	FMassProcessorExecWrapper(){};
	FMassProcessorExecWrapper(FMassEntityManager& InEntityManager, FMassExecutionContext& InContext)
	:EntityManager(&InEntityManager)
	,Context(&InContext)
	{		
	};
	
	FMassEntityManager* EntityManager {nullptr};
	FMassExecutionContext* Context {nullptr};
};


DECLARE_DYNAMIC_DELEGATE_OneParam(FExecuteOnChunk, FMassProcessorExecWrapper, InExecWrapper);