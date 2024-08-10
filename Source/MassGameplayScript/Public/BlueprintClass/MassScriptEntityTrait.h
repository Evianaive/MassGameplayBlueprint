// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "UObject/Object.h"
#include "MassScriptEntityTrait.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Abstract)
class MASSGAMEPLAYSCRIPT_API UMassScriptEntityTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()
public:

	/** Appends items into the entity template required for the trait. */
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
	UFUNCTION(BlueprintNativeEvent)
	void BuildTemplateBP();

	virtual void DestroyTemplate() const override;
	/** Called when all Traits have had BuildTemplate() called. */
	virtual void ValidateTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

	UFUNCTION(BlueprintCallable,Category="MassScriptEntityTrait",CustomThunk,meta=(CustomStructureParam="Structure"))
	bool HasTraitContains(UPARAM(ref)const int32& Structure);
	DECLARE_FUNCTION(execHasTraitContains);
	
	UFUNCTION(BlueprintCallable,Category="MassScriptEntityTrait",CustomThunk,meta=(CustomStructureParam="Structure",BlueprintInternalUseOnly="true"))
	void AddToTrait(UPARAM(ref)const int32& Structure, bool bAddDependency, bool bConst);
	DECLARE_FUNCTION(execAddToTrait);
	void Generic_AddToTrait(const FStructProperty* Property, uint8 const* PropertyValue, bool IsConst = false) const;
	
protected:
	struct FLocalDecorator* TempBuildContext;
	const UWorld* TempWorld;
};