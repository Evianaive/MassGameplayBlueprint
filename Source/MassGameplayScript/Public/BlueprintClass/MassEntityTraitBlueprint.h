// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "UObject/Object.h"
#include "MassEntityTraitBlueprint.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Abstract)
class MASSGAMEPLAYSCRIPT_API UMassEntityTraitBlueprint : public UMassEntityTraitBase
{
	GENERATED_BODY()
public:

	/** Appends items into the entity template required for the trait. */
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

	virtual void DestroyTemplate() const override;

	/** Called when all Traits have had BuildTemplate() called. */
	virtual void ValidateTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};