// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_BreakStructWithConvertMember.h"
// Copyright Epic Games, Inc. All Rights Reserved.

#include "K2Node_BreakStructWithConvertMember.h"

#include "BPTerminal.h"
#include "BlueprintEditorSettings.h"
#include "EdGraphUtilities.h"
#include "KismetCompiledFunctionContext.h"
#include "Engine/UserDefinedStruct.h"
#include "KismetCompiler.h"
#include "UserDefinedStructure/UserDefinedStructEditorData.h"

#define LOCTEXT_NAMESPACE "K2Node_BreakStructWithConvertMember"

//////////////////////////////////////////////////////////////////////////
// FKCHandler_BreakStructWithConvertMember

class FKCHandler_BreakStructWithConvertMember : public FNodeHandlingFunctor
{
public:
	FKCHandler_BreakStructWithConvertMember(FKismetCompilerContext& InCompilerContext)
		: FNodeHandlingFunctor(InCompilerContext)
	{
	}

	FBPTerminal* RegisterInputTerm(FKismetFunctionContext& Context, UK2Node_BreakStructWithConvertMember* Node)
	{
		check(NULL != Node);

		if(NULL == Node->StructType)
		{
			CompilerContext.MessageLog.Error(*LOCTEXT("BreakStruct_UnknownStructure_Error", "Unknown structure to break for @@").ToString(), Node);
			return NULL;
		}

		//Find input pin
		UEdGraphPin* InputPin = NULL;
		for (int32 PinIndex = 0; PinIndex < Node->Pins.Num(); ++PinIndex)
		{
			UEdGraphPin* Pin = Node->Pins[PinIndex];
			if(Pin && (EGPD_Input == Pin->Direction))
			{
				InputPin = Pin;
				break;
			}
		}
		check(NULL != InputPin);

		//Find structure source net
		UEdGraphPin* Net = FEdGraphUtilities::GetNetFromPin(InputPin);
		check(NULL != Net);

		FBPTerminal** FoundTerm = Context.NetMap.Find(Net);
		FBPTerminal* Term = FoundTerm ? *FoundTerm : NULL;
		if(NULL == Term)
		{
			// Dont allow literal
			if ((Net->Direction == EGPD_Input) && (Net->LinkedTo.Num() == 0))
			{
				CompilerContext.MessageLog.Error(*LOCTEXT("InvalidNoInputStructure_Error", "No input structure to break for @@").ToString(), Net);
				return NULL;
			}
			// standard register net
			else
			{
				Term = Context.CreateLocalTerminalFromPinAutoChooseScope(Net, Context.NetNameMap->MakeValidName(Net));
			}
			Context.NetMap.Add(Net, Term);
		}
		UStruct* StructInTerm = Cast<UStruct>(Term->Type.PinSubCategoryObject.Get());
		if(NULL == StructInTerm || !StructInTerm->IsChildOf(Node->StructType))
		{
			CompilerContext.MessageLog.Error(*LOCTEXT("BreakStruct_NoMatch_Error", "Structures don't match for @@").ToString(), Node);
		}

		return Term;
	}

	void RegisterOutputTerm(FKismetFunctionContext& Context, UScriptStruct* StructType, UEdGraphPin* Net, FBPTerminal* ContextTerm)
	{
		if (FProperty* BoundProperty = FindFProperty<FProperty>(StructType, Net->PinName))
		{
			if (BoundProperty->HasAnyPropertyFlags(CPF_Deprecated) && Net->LinkedTo.Num())
			{
				FText Message = FText::Format(LOCTEXT("BreakStruct_DeprecatedField_Warning", "@@ : Member '{0}' of struct '{1}' is deprecated.")
					, BoundProperty->GetDisplayNameText()
					, StructType->GetDisplayNameText());
				CompilerContext.MessageLog.Warning(*Message.ToString(), Net->GetOuter());
			}

			UBlueprintEditorSettings* Settings = GetMutableDefault<UBlueprintEditorSettings>();
			FBPTerminal* Term = Context.CreateLocalTerminalFromPinAutoChooseScope(Net, Net->PinName.ToString());
			Term->bPassedByReference = ContextTerm->bPassedByReference;
			Term->AssociatedVarProperty = BoundProperty;
/*Modify Property by Connected PinType*/
			// if(auto ArrayProperty = CastField<FArrayProperty>(Term->AssociatedVarProperty))
			// {
			// 	if(auto InnerStructProperty = CastField<FStructProperty>(ArrayProperty->Inner))
			// 	{
			// 		InnerStructProperty->Struct = Cast<UScriptStruct>(Net->PinType.PinSubCategoryObject);
			// 		InnerStructProperty->ElementSize = InnerStructProperty->Struct->GetStructureSize();
			// 	}
			// }
			
			Context.NetMap.Add(Net, Term);
			Term->Context = ContextTerm;

			if (BoundProperty->HasAnyPropertyFlags(CPF_BlueprintReadOnly))
			{
				Term->bIsConst = true;
			}
		}
		else
		{
			CompilerContext.MessageLog.Error(TEXT("Failed to find a struct member for @@"), Net);
		}
	}

	virtual void RegisterNets(FKismetFunctionContext& Context, UEdGraphNode* InNode) override
	{
		UK2Node_BreakStructWithConvertMember* Node = Cast<UK2Node_BreakStructWithConvertMember>(InNode);
		check(Node);

		if(!UK2Node_BreakStructWithConvertMember::CanBeBroken(Node->StructType, Node->IsIntermediateNode()))
		{
			CompilerContext.MessageLog.Warning(*LOCTEXT("BreakStruct_NoBreak_Error", "The structure cannot be broken using generic 'break' node @@. Try use specialized 'break' function if available.").ToString(), Node);
		}

		if(FBPTerminal* StructContextTerm = RegisterInputTerm(Context, Node))
		{
			for (UEdGraphPin* Pin : Node->Pins)
			{
				if(Pin && EGPD_Output == Pin->Direction)
				{
					RegisterOutputTerm(Context, Node->StructType, Pin, StructContextTerm);
				}
			}
		}
	}
};


UK2Node_BreakStructWithConvertMember::UK2Node_BreakStructWithConvertMember(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bMadeAfterOverridePinRemoval(false)
{
}

static bool CanCreatePinForProperty(const FProperty* Property)
{
	const UEdGraphSchema_K2* Schema = GetDefault<UEdGraphSchema_K2>();
	FEdGraphPinType DumbGraphPinType;
	const bool bConvertable = Schema->ConvertPropertyToPinType(Property, /*out*/ DumbGraphPinType);

	const bool bVisible = (Property && Property->HasAnyPropertyFlags(CPF_BlueprintVisible));
	return bVisible && bConvertable;
}

void UK2Node_BreakStructWithConvertMember::FStructOperationOptionalPinManager::CustomizePinData(UEdGraphPin* Pin,
	FName SourcePropertyName, int32 ArrayIndex, FProperty* Property) const
{
	FOptionalPinManager::CustomizePinData(Pin, SourcePropertyName, ArrayIndex, Property);

	if (Pin && Property)
	{
		const UUserDefinedStruct* UDStructure = Cast<const UUserDefinedStruct>(Property->GetOwnerStruct());
		if (UDStructure && UDStructure->EditorData)
		{
			const FStructVariableDescription* VarDesc = FStructureEditorUtils::GetVarDesc(UDStructure).FindByPredicate(
				FStructureEditorUtils::FFindByNameHelper<FStructVariableDescription>(Property->GetFName()));
			if (VarDesc)
			{
				Pin->PersistentGuid = VarDesc->VarGuid;
			}
		}
	}
}

bool UK2Node_BreakStructWithConvertMember::DoRenamedPinsMatch(const UEdGraphPin* NewPin, const UEdGraphPin* OldPin,
	bool bStructInVariablesOut) const
{
	if (NewPin && OldPin && (OldPin->Direction == NewPin->Direction))
	{
		const EEdGraphPinDirection StructDirection = bStructInVariablesOut ? EGPD_Input : EGPD_Output;
		const EEdGraphPinDirection VariablesDirection = bStructInVariablesOut ? EGPD_Output : EGPD_Input;
		const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>(); 
		const bool bCompatible = K2Schema && K2Schema->ArePinTypesCompatible(NewPin->PinType, OldPin->PinType);

		if (!bCompatible)
		{
			return false;
		}
		else if (StructDirection == OldPin->Direction)
		{
			// Struct name was changed, which is fine
			return true;
		}
		else if (VariablesDirection == OldPin->Direction)
		{
			// Name of a member variable was changed, check guids and redirects
			if ((NewPin->PersistentGuid == OldPin->PersistentGuid) && OldPin->PersistentGuid.IsValid())
			{
				return true;
			}

			if (DoesRenamedVariableMatch(OldPin->PinName, NewPin->PinName, StructType))
			{
				return true;
			}
		}
	}
	return false;
}

bool UK2Node_BreakStructWithConvertMember::CanBeBroken(const UScriptStruct* Struct, const bool bForInternalUse)
{
	if (Struct && !Struct->HasMetaData(FBlueprintMetadata::MD_NativeBreakFunction) && UEdGraphSchema_K2::IsAllowableBlueprintVariableType(Struct, bForInternalUse))
	{
		for (TFieldIterator<FProperty> It(Struct); It; ++It)
		{
			if (CanCreatePinForProperty(*It))
			{
				return true;
			}
		}
	}
	return false;
}

void UK2Node_BreakStructWithConvertMember::AllocateDefaultPins()
{
	if (StructType)
	{
		UEdGraphNode::FCreatePinParams PinParams;
		PinParams.bIsConst = true;
		PinParams.bIsReference = true;

		PreloadObject(StructType);
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, StructType, StructType->GetFName(), PinParams);
		
		struct FBreakStructPinManager : public FStructOperationOptionalPinManager
		{
			virtual bool CanTreatPropertyAsOptional(FProperty* TestProperty) const override
			{
				return CanCreatePinForProperty(TestProperty);
			}
		};

		{
			FBreakStructPinManager OptionalPinManager;
			OptionalPinManager.RebuildPropertyList(ShowPinForProperties, StructType);
			OptionalPinManager.CreateVisiblePins(ShowPinForProperties, StructType, EGPD_Output, this);
		}

		// When struct has a lot of fields, mark their pins as advanced
		if(Pins.Num() > 5)
		{
			if(ENodeAdvancedPins::NoPins == AdvancedPinDisplay)
			{
				AdvancedPinDisplay = ENodeAdvancedPins::Hidden;
			}

			for(int32 PinIndex = 3; PinIndex < Pins.Num(); ++PinIndex)
			{
				if(UEdGraphPin * EdGraphPin = Pins[PinIndex])
				{
					EdGraphPin->bAdvancedView = true;
				}
			}
		}
	}
}

void UK2Node_BreakStructWithConvertMember::ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const
{
	UK2Node::ValidateNodeDuringCompilation(MessageLog);

	if(!StructType)
	{
		MessageLog.Error(*LOCTEXT("NoStruct_Error", "No Struct in @@").ToString(), this);
	}
	else
	{
		bool bHasAnyBlueprintVisibleProperty = false;
		for (TFieldIterator<FProperty> It(StructType); It; ++It)
		{
			const FProperty* Property = *It;
			if (CanCreatePinForProperty(Property))
			{
				const bool bIsBlueprintVisible = Property->HasAnyPropertyFlags(CPF_BlueprintVisible) || (Property->GetOwnerStruct() && Property->GetOwnerStruct()->IsA<UUserDefinedStruct>());
				bHasAnyBlueprintVisibleProperty |= bIsBlueprintVisible;

				const UEdGraphPin* Pin = FindPin(Property->GetFName());
				const bool bIsLinked = Pin && Pin->LinkedTo.Num();

				if (!bIsBlueprintVisible && bIsLinked)
				{
					MessageLog.Warning(*LOCTEXT("PropertyIsNotBPVisible_Warning", "@@ - the native property is not tagged as BlueprintReadWrite or BlueprintReadOnly, the pin will be removed in a future release.").ToString(), Pin);
				}

				if ((Property->ArrayDim > 1) && bIsLinked)
				{
					MessageLog.Warning(*LOCTEXT("StaticArray_Warning", "@@ - the native property is a static array, which is not supported by blueprints").ToString(), Pin);
				}
			}
		}

		if (!bHasAnyBlueprintVisibleProperty)
		{
			MessageLog.Warning(*LOCTEXT("StructHasNoBPVisibleProperties_Warning", "@@ has no property tagged as BlueprintReadWrite or BlueprintReadOnly. The node will be removed in a future release.").ToString(), this);
		}

		if (!bMadeAfterOverridePinRemoval)
		{
			MessageLog.Warning(*NSLOCTEXT("K2Node", "OverridePinRemoval_BreakStructWithConvertMember", "Override pins have been removed from @@ in @@, please verify the Blueprint works as expected! See tooltips for enabling pin visibility for more details. This warning will go away after you resave the asset!").ToString(), this, GetBlueprint());
		}
	}
}

UK2Node::ERedirectType UK2Node_BreakStructWithConvertMember::DoPinsMatchForReconstruction(const UEdGraphPin* NewPin, int32 NewPinIndex, const UEdGraphPin* OldPin, int32 OldPinIndex) const
{
	ERedirectType Result = UK2Node::DoPinsMatchForReconstruction(NewPin, NewPinIndex, OldPin, OldPinIndex);
	if ((ERedirectType_None == Result) && DoRenamedPinsMatch(NewPin, OldPin, true))
	{
		Result = ERedirectType_Name;
	}

	return Result;
}

FNodeHandlingFunctor* UK2Node_BreakStructWithConvertMember::CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const
{
	return new FKCHandler_BreakStructWithConvertMember(CompilerContext);
}

void UK2Node_BreakStructWithConvertMember::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	// This Node Can't be created by User
	return;
}

void UK2Node_BreakStructWithConvertMember::PostPlacedNewNode()
{
	Super::PostPlacedNewNode();

	// New nodes automatically have this set.
	bMadeAfterOverridePinRemoval = true;
}

#undef LOCTEXT_NAMESPACE
