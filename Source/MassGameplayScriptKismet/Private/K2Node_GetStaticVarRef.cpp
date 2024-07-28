// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_GetStaticVarRef.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraphUtilities.h"
#include "KismetCompiledFunctionContext.h"
#include "KismetCompilerMisc.h"
#include "TestPassRefLibrary.h"

#define LOCTEXT_NAMESPACE "GetStaticVarRef"

class FKCHandler_GetStaticVarRef : public FNodeHandlingFunctor
{
public:
	FKCHandler_GetStaticVarRef(FKismetCompilerContext& InCompilerContext)
		: FNodeHandlingFunctor(InCompilerContext)
	{
	}
	virtual void RegisterNets(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		UK2Node_GetStaticVarRef* ArrayNode = CastChecked<UK2Node_GetStaticVarRef>(Node);

		// return inline term
		auto OutputPin = ArrayNode->GetOutputPin();
		
		if (Context.NetMap.Find(OutputPin))
		{
			Context.MessageLog.Error(*LOCTEXT("Error_ReturnTermAlreadyRegistered", "ICE: Return term is already registered @@").ToString(), Node);
			return;
		}

		{
			FBPTerminal* Term = new FBPTerminal();
			// Currently this Array is not used!
			Context.InlineGeneratedValues.Add(Term);
			Term->CopyFromPin(OutputPin, Context.NetNameMap->MakeValidName(OutputPin));
			Context.NetMap.Add(OutputPin, Term);
		}

		/* auto register other pin(literal) by default method*/ 
		FNodeHandlingFunctor::RegisterNets(Context, Node);
	}
	virtual void Compile(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		UK2Node_GetStaticVarRef* ArrayNode = CastChecked<UK2Node_GetStaticVarRef>(Node);

		FBlueprintCompiledStatement* ArrayGetFunction = new FBlueprintCompiledStatement();
		ArrayGetFunction->Type = KCST_CallFunction;
		ArrayGetFunction->FunctionToCall = UTestPassRefLibrary::StaticClass()->FindFunctionByName(TEXT("GetTestStructRef"));
		
		UEdGraphPin* ReturnValueNet = FEdGraphUtilities::GetNetFromPin(ArrayNode->GetOutputPin());
		FBPTerminal** ReturnValue = Context.NetMap.Find(ReturnValueNet);

		(*ReturnValue)->InlineGeneratedParameter = ArrayGetFunction;
	}
};

void UK2Node_GetStaticVarRef::AllocateDefaultPins()
{
	// Create the output pin
	UEdGraphNode::FCreatePinParams OutputPinParams;
	OutputPinParams.bIsReference = bReturnByRefDesired;
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, TEXT("Output"), OutputPinParams);
}

void UK2Node_GetStaticVarRef::PostReconstructNode()
{
	Super::PostReconstructNode();
}

FText UK2Node_GetStaticVarRef::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return Super::GetNodeTitle(TitleType);
}

void UK2Node_GetStaticVarRef::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::NotifyPinConnectionListChanged(Pin);
	if (Pin == GetOutputPin() && Pin->ParentPin == nullptr)
	{
		if (Pin->LinkedTo.Num() > 0)
		{
			if (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
			{
				FEdGraphPinType& NewType = Pin->LinkedTo[0]->PinType;
				if (NewType.PinCategory != UEdGraphSchema_K2::PC_Wildcard)
				{
					Pin->PinType = NewType;
					Pin->PinType.bIsReference = true;
					GetGraph()->NotifyNodeChanged(this);
				}
			}
		}
		else
		{
			// link break
			Pin->PinType.PinCategory = UEdGraphSchema_K2::PC_Wildcard;
			Pin->PinType.PinSubCategory = NAME_None;
			Pin->PinType.PinSubCategoryObject = nullptr;
			Pin->PinType.bIsReference = bReturnByRefDesired;

			Pin->BreakAllPinLinks();
		}
	}
}

void UK2Node_GetStaticVarRef::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	Super::ReallocatePinsDuringReconstruction(OldPins);
}

void UK2Node_GetStaticVarRef::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FNodeHandlingFunctor* UK2Node_GetStaticVarRef::CreateNodeHandler(FKismetCompilerContext& CompilerContext) const
{
	return new FKCHandler_GetStaticVarRef(CompilerContext);
}

UEdGraphPin* UK2Node_GetStaticVarRef::GetOutputPin() const
{
	return Pins[0];
}

#undef LOCTEXT_NAMESPACE