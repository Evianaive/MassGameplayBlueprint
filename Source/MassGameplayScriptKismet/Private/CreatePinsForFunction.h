﻿
#pragma once 

#include "K2Node_CallFunction.h"
#include "Kismet2/BlueprintEditorUtils.h"

class K2NodeHelper
{
public:
	static bool CreatePinsForFunctionEntryExit(UK2Node* InNode, const UFunction* Function, bool bForFunctionEntry)
	{
		const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

		// if the generated class is not up-to-date, use the skeleton's class function to create pins:
		Function = FBlueprintEditorUtils::GetMostUpToDateFunction(Function);

		// Create the inputs and outputs
		bool bAllPinsGood = true;
		for (TFieldIterator<FProperty> PropIt(Function); PropIt && (PropIt->PropertyFlags & CPF_Parm); ++PropIt)
		{
			FProperty* Param = *PropIt;

			// Don't create a new pin if one exists already! 
			// @see UE-79032, UE-58390
			if (InNode->FindPin(Param->GetFName()))
			{
				continue;
			}

			const bool bIsFunctionInput = !Param->HasAnyPropertyFlags(CPF_OutParm) || Param->HasAnyPropertyFlags(CPF_ReferenceParm);

			if (bIsFunctionInput == bForFunctionEntry)
			{
				const EEdGraphPinDirection Direction = bForFunctionEntry ? EGPD_Input : EGPD_Output;

				UEdGraphPin* Pin = InNode->CreatePin(Direction, NAME_None, Param->GetFName());
				const bool bPinGood = K2Schema->ConvertPropertyToPinType(Param, /*out*/ Pin->PinType);
				K2Schema->SetPinAutogeneratedDefaultValueBasedOnType(Pin);
			
				UK2Node_CallFunction::GeneratePinTooltipFromFunction(*Pin, Function);

				bAllPinsGood = bAllPinsGood && bPinGood;
			}
		}

		return bAllPinsGood;
	}
};
