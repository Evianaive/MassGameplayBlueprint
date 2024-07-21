// Fill out your copyright notice in the Description page of Project Settings.


#include "TestPassRefLibrary.h"

#define LOCTEXT_NAMESPACE "TestPassRefLibrary"


FTestReturnByRef::FTestReturnByRef()
{
	TestInt = 100;
}

FTestReturnByRef& UTestPassRefLibrary::GetTestStructRef()
{
	return InlineTestStruct;
};

DEFINE_FUNCTION(UTestPassRefLibrary::execGetTestStructRef)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	// auto Parm = *((void**)&RESULT_PARAM);
	if(RESULT_PARAM)
	{
		*(FTestReturnByRef*)RESULT_PARAM = GetTestStructRef();
	}
	Stack.MostRecentPropertyAddress = reinterpret_cast<uint8*>(&GetTestStructRef());
	static FStructProperty Prop = FStructProperty(EC_InternalUseOnlyConstructor,nullptr);
	Stack.MostRecentProperty = &Prop;
	P_NATIVE_END;
}

FTestReturnByRef& UTestPassRefLibrary::GetTestStructRefPure()
{
	return InlineTestStruct;
}

void UTestPassRefLibrary::SetTestStructRef(FTestReturnByRef& Struct)
{
	Struct.TestInt = 6;
}

#undef LOCTEXT_NAMESPACE
