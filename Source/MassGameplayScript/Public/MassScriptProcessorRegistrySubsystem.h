// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassSubsystemBase.h"
#include "UObject/Object.h"
#include "MassScriptProcessorRegistrySubsystem.generated.h"

/**
 * 
 */
UCLASS()
class MASSGAMEPLAYSCRIPT_API UMassScriptProcessorRegistrySubsystem : public UMassSubsystemBase
{
	GENERATED_BODY()
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};
