// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_GetArrayViewItem.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "CreatePinsForFunction.h"
#include "EdGraphUtilities.h"
#include "KismetCompiledFunctionContext.h"
#include "Helpers/MassBlueprintLibrary.h"
#include "SPinTypeSelector.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Widgets/Images/SLayeredImage.h"

#define LOCTEXT_NAMESPACE "GetArrayViewItem"


class FKCHandler_GetArrayView : public FNodeHandlingFunctor
{
public:
	FKCHandler_GetArrayView(FKismetCompilerContext& InCompilerContext)
		: FNodeHandlingFunctor(InCompilerContext)
	{
	}

	TMap<UEdGraphNode*,FBPTerminal*> NodeTermMap;
	virtual void RegisterNets(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		UK2Node_GetArrayViewItem* ArrayNode = CastChecked<UK2Node_GetArrayViewItem>(Node);
		
		// return inline term
		auto OutputPin = ArrayNode->GetOutputPin();
		
		if (Context.NetMap.Find(OutputPin))
		{
			Context.MessageLog.Error(*LOCTEXT("Error_ReturnTermAlreadyRegistered", "ICE: Return term is already registered @@").ToString(), Node);
			return;
		}
		// output is inline generated
		{
			FBPTerminal* Term = new FBPTerminal();
			Context.InlineGeneratedValues.Add(Term);
			Term->CopyFromPin(OutputPin, Context.NetNameMap->MakeValidName(OutputPin));
			Context.NetMap.Add(OutputPin, Term);
		}
		// has non reference pin
		// bool ConnectionHasNonRef = false;
		// for(auto LinkedTo : OutputPin->LinkedTo)
		// {
		// 	ConnectionHasNonRef |= !LinkedTo->PinType.bIsReference;
		// }
		// if(!ConnectionHasNonRef)
		// 	return;
		
		// Create Non Reference Pin
		UEdGraphPin* NonReferencePin = UEdGraphPin::CreatePin(ArrayNode);

		NonReferencePin->Direction = EEdGraphPinDirection::EGPD_Output;
		NonReferencePin->PinName = FName(OutputPin->PinName.ToString()+TEXT("_NonRef"));		
		NonReferencePin->PinType = OutputPin->PinType;
		NonReferencePin->PinType.bIsReference = false;

		RegisterNet(Context,NonReferencePin);
		if(FBPTerminal** NonRefTerm = Context.NetMap.Find(NonReferencePin))
		{
			NodeTermMap.Add(ArrayNode,*NonRefTerm);
		}
		FNodeHandlingFunctor::RegisterNets(Context, Node);
	}
	virtual void RegisterNet(FKismetFunctionContext& Context, UEdGraphPin* Pin) override
	{
		// This net is an output from a function call
		FBPTerminal* Term = Context.CreateLocalTerminalFromPinAutoChooseScope(Pin, Context.NetNameMap->MakeValidName(Pin));
		Context.NetMap.Add(Pin, Term);
	}
	virtual void Compile(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		UK2Node_GetArrayViewItem* ArrayNode = CastChecked<UK2Node_GetArrayViewItem>(Node);
		
		auto Func = UMassBlueprintLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UMassBlueprintLibrary, GetStructRef));
		FBlueprintCompiledStatement* NewStatement = new FBlueprintCompiledStatement();
		NewStatement->Type = KCST_CallFunction;
		NewStatement->FunctionToCall = Func;
		
		UEdGraphPin* ArrayViewNet = FEdGraphUtilities::GetNetFromPin(ArrayNode->GetArrayViewPin());
		FBPTerminal** ArrayViewTerm = Context.NetMap.Find(ArrayViewNet);
		
		UEdGraphPin* IndexPinNet = FEdGraphUtilities::GetNetFromPin(ArrayNode->GetIndexPin());
		FBPTerminal** IndexTermPtr = Context.NetMap.Find(IndexPinNet);

		NewStatement->RHS.Add(*ArrayViewTerm);
		NewStatement->RHS.Add(*IndexTermPtr);
		FBPTerminal** NonRefTerm = NodeTermMap.Find(ArrayNode);
		NewStatement->RHS.Add(*NonRefTerm);

		UEdGraphPin* ReturnValueNet = FEdGraphUtilities::GetNetFromPin(ArrayNode->GetOutputPin());
		FBPTerminal** ReturnValue = Context.NetMap.Find(ReturnValueNet);

		(*ReturnValue)->InlineGeneratedParameter = NewStatement;
	}
};

void UK2Node_GetArrayViewItem::AllocateDefaultPins()
{
	auto Func = UMassBlueprintLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UMassBlueprintLibrary, GetStructRef));
	K2NodeHelper::CreatePinsForFunctionEntryExit(this,Func,true);
	// FBlueprintNodeStatics::CreateParameterPinsForFunction(this,Func,[this](UEdGraphPin* NewPin){});
	// Create the output pin
	UEdGraphNode::FCreatePinParams OutputPinParams;
	OutputPinParams.bIsReference = bReturnByRefDesired;
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, Output, OutputPinParams);
}

void UK2Node_GetArrayViewItem::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	Super::PinDefaultValueChanged(Pin);
}

void UK2Node_GetArrayViewItem::PostReconstructNode()
{
	Super::PostReconstructNode();
	
	UEdGraphPin* OutputPin = GetOutputPin();
	if (OutputPin->LinkedTo.Num() > 0)
	{
		OutputPin->PinType = OutputPin->LinkedTo[0]->PinType;
		OutputPin->PinType.bIsReference = true;
	}	
}

FText UK2Node_GetArrayViewItem::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::FullTitle)
	{
		return LOCTEXT("GetArrayViewItemByRef_FullTitle", "GET ARRAY VIEW new");
	}
	return LOCTEXT("GetArrayViewItemByRef", "Get From ArrayView (a ref) new");
}

TSharedPtr<SWidget> UK2Node_GetArrayViewItem::CreateNodeImage() const
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

FNodeHandlingFunctor* UK2Node_GetArrayViewItem::CreateNodeHandler(FKismetCompilerContext& CompilerContext) const
{
	return new FKCHandler_GetArrayView(CompilerContext);
}

void UK2Node_GetArrayViewItem::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
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
			GetGraph()->NotifyNodeChanged(this);
		}
	}
}

void UK2Node_GetArrayViewItem::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	Super::ReallocatePinsDuringReconstruction(OldPins);
}

FText UK2Node_GetArrayViewItem::GetMenuCategory() const
{
	// Todo add to MassBlueprintLibrary
	// return Super::GetMenuCategory();
	return LOCTEXT("ArrayUtilitiesCategory", "Utilities|Array");;
}

void UK2Node_GetArrayViewItem::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

void UK2Node_GetArrayViewItem::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);
}

bool UK2Node_GetArrayViewItem::IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin,
	FString& OutReason) const
{
	return Super::IsConnectionDisallowed(MyPin, OtherPin, OutReason);
}

void UK2Node_GetArrayViewItem::PreloadRequiredAssets()
{
	Super::PreloadRequiredAssets();
}

#undef LOCTEXT_NAMESPACE