﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_GetArrayViewItemRef.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "K2Node_BreakStructWithConvertMember.h"
#include "K2Node_CallFunction.h"
#include "K2Node_GetArrayItem.h"
#include "KismetCompiler.h"
#include "MassBlueprintLibrary.h"
#include "SPinTypeSelector.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Widgets/Images/SLayeredImage.h"

#define LOCTEXT_NAMESPACE "GetArrayViewItemRef"

bool CreatePinsForFunctionEntryExit(UK2Node* InNode, const UFunction* Function, bool bForFunctionEntry)
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	// if the generated class is not up to date, use the skeleton's class function to create pins:
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

void UK2Node_GetArrayViewItemRef::AllocateDefaultPins()
{
	auto Func = UMassBlueprintLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UMassBlueprintLibrary, GetStructRef));
	::CreatePinsForFunctionEntryExit(this,Func,true);

	// Create the output pin
	UEdGraphNode::FCreatePinParams OutputPinParams;
	OutputPinParams.bIsReference = bReturnByRefDesired;
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, Output, OutputPinParams);
}

void UK2Node_GetArrayViewItemRef::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	Super::PinDefaultValueChanged(Pin);
}

void UK2Node_GetArrayViewItemRef::PostReconstructNode()
{
	Super::PostReconstructNode();
	
	UEdGraphPin* OutputPin = GetOutputPin();
	if (OutputPin->LinkedTo.Num() > 0)
	{
		OutputPin->PinType = OutputPin->LinkedTo[0]->PinType;
		OutputPin->PinType.bIsReference = true;
	}	
}

FText UK2Node_GetArrayViewItemRef::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::FullTitle)
	{
		return LOCTEXT("GetArrayViewItemByRef_FullTitle", "GET ARRAY VIEW");
	}
	return LOCTEXT("GetArrayViewItemByRef", "Get From ArrayView (a ref)");
}

TSharedPtr<SWidget> UK2Node_GetArrayViewItemRef::CreateNodeImage() const
{
	auto OutputPin = GetOutputPin();
	// Icon bindings:
	TAttribute<const FSlateBrush*> PrimaryIcon = TAttribute<const FSlateBrush*>::Create(
		TAttribute<const FSlateBrush*>::FGetter::CreateLambda(
			[OutputPin]() -> const FSlateBrush*
			{
				if(!OutputPin->IsPendingKill())
				{
					auto TempPinType = OutputPin->PinType;
					TempPinType.ContainerType = EPinContainerType::Array;
					return FBlueprintEditorUtils::GetIconFromPin(TempPinType, /* bIsLarge = */true);
				}
				return nullptr;
			}
		)
	);	
	auto TempCenterWidget = SPinTypeSelector::ConstructPinTypeImage(GetOutputPin());
	auto CenterWidget = StaticCastSharedRef<SLayeredImage>(TempCenterWidget);
	CenterWidget->SetLayerBrush(0,PrimaryIcon);
	return CenterWidget;
}

void UK2Node_GetArrayViewItemRef::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
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

void UK2Node_GetArrayViewItemRef::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	Super::ReallocatePinsDuringReconstruction(OldPins);
}

FText UK2Node_GetArrayViewItemRef::GetMenuCategory() const
{
	// Todo add to MassBlueprintLibrary
	// return Super::GetMenuCategory();
	return LOCTEXT("ArrayUtilitiesCategory", "Utilities|Array");;
}

void UK2Node_GetArrayViewItemRef::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

void UK2Node_GetArrayViewItemRef::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);
	auto IndexPin = GetIndexPin();
	auto ArrayViewPin = FindPinByPredicate([](const UEdGraphPin* InPin)
	{
		return InPin->PinType.PinCategory==UEdGraphSchema_K2::PC_Struct;
	});
	UEdGraphPin* OutStructPin = GetOutputPin();

	UK2Node_GetArrayItem* GetArrayNode = CompilerContext.SpawnIntermediateNode<UK2Node_GetArrayItem>(this, SourceGraph);
	// GetArrayNode->SetDesiredReturnType(true);
	GetArrayNode->AllocateDefaultPins();
	GetArrayNode->GetResultPin()->PinType.bIsReference = true;
	
	auto* BreakArrayViewNode = CompilerContext.SpawnIntermediateNode<UK2Node_BreakStructWithConvertMember>(this,SourceGraph);
	BreakArrayViewNode->StructType = FArrayViewBlueprint::StaticStruct();
	BreakArrayViewNode->bMadeAfterOverridePinRemoval = true;
	
	BreakArrayViewNode->AllocateDefaultPins();
	// Change Pin Type
	if(auto BreakArrayViewNodeOutput = BreakArrayViewNode->GetPinWithDirectionAt(0,EGPD_Output))
	{
		BreakArrayViewNodeOutput->PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
		BreakArrayViewNodeOutput->PinType.PinSubCategoryObject = OutStructPin->PinType.PinSubCategoryObject;

		// IndexPin->AssignByRefPassThroughConnection()
		// 
		
		BreakArrayViewNodeOutput->MakeLinkTo(GetArrayNode->GetPinWithDirectionAt(0,EGPD_Input));
		CompilerContext.MovePinLinksToIntermediate(*IndexPin,*GetArrayNode->GetPinWithDirectionAt(1,EGPD_Input));
		CompilerContext.MovePinLinksToIntermediate(*OutStructPin,*GetArrayNode->GetPinWithDirectionAt(0,EGPD_Output));
		// auto BreakArrayViewNodeInput = BreakArrayViewNode->GetPinWithDirectionAt(0,EGPD_Input)
		CompilerContext.MovePinLinksToIntermediate(*ArrayViewPin,*BreakArrayViewNode->GetPinWithDirectionAt(0,EGPD_Input));
		GetArrayNode->PostReconstructNode();
	}
	
	// ArrayViewPin->LinkedTo[0];
	
	// ArrayViewPin->LinkedTo
}

bool UK2Node_GetArrayViewItemRef::IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin,
	FString& OutReason) const
{
	return Super::IsConnectionDisallowed(MyPin, OtherPin, OutReason);
}

void UK2Node_GetArrayViewItemRef::PreloadRequiredAssets()
{
	Super::PreloadRequiredAssets();
}

#undef LOCTEXT_NAMESPACE