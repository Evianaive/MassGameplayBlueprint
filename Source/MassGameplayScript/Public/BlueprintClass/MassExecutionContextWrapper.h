// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "MassExecutionContextWrapper.generated.h"

struct FMassEntityManager;
struct FMassExecutionContext;
USTRUCT(BlueprintType)
struct FMassExecutionContextWrapper
{
	GENERATED_BODY()
	FMassExecutionContextWrapper(){};
	FMassExecutionContextWrapper(FMassEntityManager& InEntityManager, FMassExecutionContext& InContext)
	:EntityManager(&InEntityManager)
	,Context(&InContext)
	{		
	};
	
	FMassEntityManager* EntityManager {nullptr};
	FMassExecutionContext* Context {nullptr};
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FExecuteOnChunk, FMassExecutionContextWrapper, InExecWrapper);