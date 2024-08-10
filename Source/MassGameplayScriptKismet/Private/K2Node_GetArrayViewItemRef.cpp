// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_GetArrayViewItemRef.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintNodeStatics.h"
#include "CreatePinsForFunction.h"
#include "K2Node_BreakStructWithConvertMember.h"
#include "K2Node_GetArrayItem.h"
#include "KismetCompiler.h"
#include "Helpers/MassBlueprintLibrary.h"
#include "SPinTypeSelector.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Widgets/Images/SLayeredImage.h"

#define LOCTEXT_NAMESPACE "GetArrayViewItemRef"


void UK2Node_GetArrayViewItemRef::AllocateDefaultPins()
{
	auto Func = UMassBlueprintLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UMassBlueprintLibrary, GetStructRef));
	K2NodeHelper::CreatePinsForFunctionEntryExit(this,Func,true);
	// FBlueprintNodeStatics::CreateParameterPinsForFunction(this,Func,[this](UEdGraphPin* NewPin){});
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
			GetGraph()->NotifyNodeChanged(this);
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
	UEdGraphPin* OutStructPin = GetOutputPin();
	auto ArrayViewPin = GetArrayViewPin();

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