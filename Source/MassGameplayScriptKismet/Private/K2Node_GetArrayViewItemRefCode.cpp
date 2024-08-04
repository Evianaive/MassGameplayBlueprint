// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_GetArrayViewItemRefCode.h"

#include "EdGraphUtilities.h"
#include "KismetCompiledFunctionContext.h"
#include "KismetCompiler.h"
#include "Utility/PrivateAccessor.hpp"

#define LOCTEXT_NAMESPACE "GetArrayViewItemRefCode"

DECLARE_PRIVATE_ACCESS(FProperty,Offset_Internal,int32)

class FKCHandler_GetArrayViewItemRefCode : public FNodeHandlingFunctor
{
public:
	static inline FString Ends = TEXT("_ArrayPropHackMark");
	FKCHandler_GetArrayViewItemRefCode(FKismetCompilerContext& InCompilerContext)
		: FNodeHandlingFunctor(InCompilerContext)
	{
	}
	virtual void RegisterNets(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		UK2Node_GetArrayViewItemRefCode* ArrayNode = CastChecked<UK2Node_GetArrayViewItemRefCode>(Node);

		// return inline term
		auto OutputPin = ArrayNode->GetOutputPin();
		
		if (Context.NetMap.Find(OutputPin))
		{
			Context.MessageLog.Error(*LOCTEXT("Error_ReturnTermAlreadyRegistered", "ICE: Return term is already registered @@").ToString(), Node);
			return;
		}

		{
			FBPTerminal* Term = new FBPTerminal();
			Context.InlineGeneratedValues.Add(Term);
			Term->CopyFromPin(OutputPin, Context.NetNameMap->MakeValidName(OutputPin));
			Context.NetMap.Add(OutputPin, Term);
		}
		{			
			FBPTerminal* Term = new FBPTerminal();
			if (Context.IsEventGraph())
			{
				Context.EventGraphLocals.Add(Term);
			}
			else
			{
				Context.Locals.Add(Term);
				// Context.VariableReferences.Add(Term);
			}
			
			ArrayNode->FakeArrayInputPin = UEdGraphPin::CreatePin(ArrayNode);
			
			ArrayNode->FakeArrayInputPin->PinType = OutputPin->PinType;
			ArrayNode->FakeArrayInputPin->PinType.ContainerType = EPinContainerType::Array;
			
			Term->CopyFromPin(ArrayNode->FakeArrayInputPin, ArrayNode->GetName()+Ends);
			Context.NetMap.Add(ArrayNode->FakeArrayInputPin, Term);	
		}

		/* auto register other pin(literal) by default method*/ 
		FNodeHandlingFunctor::RegisterNets(Context, Node);
	}

	TArray<FArrayProperty*> FakeArrayProps;
	TArray<FStructProperty*> StrutProps;

	virtual void Compile(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		UK2Node_GetArrayViewItemRefCode* ArrayNode = CastChecked<UK2Node_GetArrayViewItemRefCode>(Node);

		FBlueprintCompiledStatement* ArrayGetFunction = new FBlueprintCompiledStatement();
		ArrayGetFunction->Type = KCST_ArrayGetByRef;

		UEdGraphPin* ArrayViewPin = FEdGraphUtilities::GetNetFromPin(ArrayNode->GetArrayViewPin());
		FBPTerminal** ArrayViewTerm = Context.NetMap.Find(ArrayViewPin);
		
		if(ArrayViewTerm)
		{
			// auto Property = FArrayViewBlueprint::StaticStruct()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(FArrayViewBlueprint,AsArray));
			if(FBPTerminal** FakeArrayTerm = Context.NetMap.Find(ArrayNode->FakeArrayInputPin))
			{
				auto FakeArrayProp = CastField<FArrayProperty>((*FakeArrayTerm)->AssociatedVarProperty);
				auto StrutProp = CastField<FStructProperty>((*ArrayViewTerm)->AssociatedVarProperty);
				if(FakeArrayProp && StrutProp)
				{
					FakeArrayProps.Add(FakeArrayProp);
					StrutProps.Add(StrutProp);
					(*ArrayViewTerm)->AssociatedVarProperty = (*FakeArrayTerm)->AssociatedVarProperty;	
				}
			}
			else
			{
				Context.MessageLog.Error(*LOCTEXT("Error_TempTermNotFounded", "ICE: Temp local term is not founded @@").ToString(), Node);
				return;
			}
		}
		
		UEdGraphPin* ReturnValueNet = FEdGraphUtilities::GetNetFromPin(Node->Pins[2]);
		FBPTerminal** ReturnValue = Context.NetMap.Find(ReturnValueNet);
		FBPTerminal** ReturnValueOrig = Context.NetMap.Find(Node->Pins[2]);

		UEdGraphPin* IndexPin = ArrayNode->GetIndexPin();
		check(IndexPin);
		UEdGraphPin* IndexPinNet = FEdGraphUtilities::GetNetFromPin(IndexPin);
		FBPTerminal** IndexTermPtr = Context.NetMap.Find(IndexPinNet);

		ArrayGetFunction->RHS.Add(*ArrayViewTerm);
		ArrayGetFunction->RHS.Add(*IndexTermPtr);

		(*ReturnValue)->InlineGeneratedParameter = ArrayGetFunction;
	}
	void PostCompileFunctionHackProp(FKismetCompilerContext* Context)
	{
		if(FakeArrayProps.Num()>0 && FakeArrayProps.Num() == StrutProps.Num())
		{
			TSet<FFieldVariant> Owners;
			for (int i = 0; i < FakeArrayProps.Num(); ++i)
			{
				FakeArrayProps[i]->*PRIVATE_ACCESS(FProperty,Offset_Internal) = StrutProps[i]->GetOffset_ForInternal();
				Owners.Add(FakeArrayProps[i]->Owner);		
			}
			for (auto Owner : Owners)
			{
				if(!Owner.IsUObject())
					continue;
				// FField* LastAllocatedProperty = Owner.Get<UStruct>()->ChildProperties;
				for (FProperty* Property : TFieldRange<FProperty>(Owner.Get<UStruct>()))
				{
					
#define REMOVE_PROP_LINK(PROP_LINK)\
					if(Property->PROP_LINK->GetName().EndsWith(Ends))\
					{\
						Property->PROP_LINK = Property->PROP_LINK->PROP_LINK;\
					}\
					
					REMOVE_PROP_LINK(Next);
					REMOVE_PROP_LINK(DestructorLinkNext);
					REMOVE_PROP_LINK(PostConstructLinkNext);
					REMOVE_PROP_LINK(PropertyLinkNext);
#undef REMOVE_PROP_LINK
				}
			}
			FakeArrayProps.Reset();
			StrutProps.Reset();
		}
	}
};

FNodeHandlingFunctor* UK2Node_GetArrayViewItemRefCode::CreateNodeHandler(FKismetCompilerContext& CompilerContext) const
{
	auto Handler = new FKCHandler_GetArrayViewItemRefCode(CompilerContext);
	auto DelegateHandle = CompilerContext.OnFunctionListCompiled().AddRaw(Handler,&FKCHandler_GetArrayViewItemRefCode::PostCompileFunctionHackProp);
	return Handler;
}

FText UK2Node_GetArrayViewItemRefCode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::FullTitle)
	{
		return LOCTEXT("GetArrayViewItemByRefCode_FullTitle", "GET ARRAY VIEW CODE");
	}
	return LOCTEXT("GetArrayViewItemByRefCode", "Get From ArrayView Code (a ref)");
}

void UK2Node_GetArrayViewItemRefCode::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{	
}

#undef LOCTEXT_NAMESPACE