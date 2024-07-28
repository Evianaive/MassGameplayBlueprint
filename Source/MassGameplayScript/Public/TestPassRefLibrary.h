// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TestPassRefLibrary.generated.h"


USTRUCT(BlueprintType)
struct FTestReturnByRef
{
	GENERATED_BODY()
	FTestReturnByRef();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 TestInt = 0;
};

/**
 * 
 */
UCLASS()
class MASSGAMEPLAYSCRIPT_API UTestPassRefLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable,CustomThunk)
	static UPARAM(ref) FTestReturnByRef& GetTestStructRef();
	DECLARE_FUNCTION(execGetTestStructRef);

	UFUNCTION(BlueprintCallable,BlueprintPure)
	static UPARAM(ref) FTestReturnByRef& GetTestStructRefPure();
	
	UFUNCTION(BlueprintCallable)
	static void SetTestStructRef(UPARAM(ref)FTestReturnByRef& Struct);
	
	static inline FTestReturnByRef InlineTestStruct;
};
