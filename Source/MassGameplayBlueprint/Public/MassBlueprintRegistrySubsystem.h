// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassSubsystemBase.h"
#include "UObject/Object.h"
#include "MassBlueprintRegistrySubsystem.generated.h"

/**
 * 
 */
UCLASS()
class MASSGAMEPLAYBLUEPRINT_API UMassBlueprintRegistrySubsystem : public UMassSubsystemBase
{
	GENERATED_BODY()
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};
