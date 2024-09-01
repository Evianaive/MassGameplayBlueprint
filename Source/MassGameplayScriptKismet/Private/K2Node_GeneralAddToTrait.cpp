// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_GeneralAddToTrait.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintNodeStatics.h"
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"
#include "MassEntityTypes.h"
#include "BlueprintClass/MassScriptEntityTrait.h"

#define LOCTEXT_NAMESPACE "GeneralAddToTrait"

void UK2Node_GeneralAddToTrait::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
	auto Func = UMassScriptEntityTrait::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UMassScriptEntityTrait, AddToTrait));
	UEdGraphPin* SelfPin = FBlueprintNodeStatics::CreateSelfPin(this,Func);
	// K2NodeHelper::CreatePinsForFunctionEntryExit(this,Func,true);
	FBlueprintNodeStatics::CreateParameterPinsForFunction(this,Func,[this](UEdGraphPin* NewPin)
	{
		if(NewPin->PinType.PinCategory != UEdGraphSchema_K2::PC_Boolean)
			return;
		if(NewPin->PinName == TEXT("bConst"))
		{
			Pin_bConst = NewPin;
			Pin_bConst->SafeSetHidden(true);
		}
		if(NewPin->PinName == TEXT("bAddDependency"))
		{
			NewPin->DefaultValue = bAddDependency?TEXT("true"):TEXT("false");
			NewPin->SafeSetHidden(true);
		}
	});
}

void UK2Node_GeneralAddToTrait::PostReconstructNode()
{
	Super::PostReconstructNode();
	NotifyPinConnectionListChanged(GetStructPin());
}

FText UK2Node_GeneralAddToTrait::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::FullTitle)
	{
		if(GetStructPin()->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct)
		{
			UStruct* Struct = GetStruct();
			if(!Struct)
			{
				return LOCTEXT("GeneralAddToTrait_FullTitle", "Not Connected");	
			}
			FString Title;
			if(Struct->IsChildOf(FMassFragment::StaticStruct()))
				Title = TEXT("Add Fragment");
			else if(Struct->IsChildOf(FMassTag::StaticStruct()))
				Title = TEXT("Add Tag");
			else if(Struct->IsChildOf(FMassChunkFragment::StaticStruct()))
				Title = TEXT("Add ChunkFragment");
			else if(Struct->IsChildOf(FMassSharedFragment::StaticStruct()))
				Title = TEXT("Add SharedFragment");
			else
				Title = TEXT("Not Valid Struct!");			
			if(bAddDependency)
				Title.Append(TEXT(" Dependency"));				
			return FText::FromString(Title);
		}
		return LOCTEXT("GeneralAddToTrait_FullTitle", "Not Struct!");
	}
	if(bAddDependency)
		return LOCTEXT("GeneralAddToTrait", "Add Dependency To Trait");	
	return LOCTEXT("GeneralAddToTrait", "Add Value To Trait");
}

FSlateIcon UK2Node_GeneralAddToTrait::GetIconAndTint(FLinearColor& OutColor) const
{
	return UK2Node_CallFunction::GetPaletteIconForFunction(TargetFunction, OutColor);
}

void UK2Node_GeneralAddToTrait::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::NotifyPinConnectionListChanged(Pin);
	if (Pin == GetStructPin() && Pin->ParentPin == nullptr)
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
					if(Pin->PinType.PinSubCategoryObject.IsValid())
					{
						const UScriptStruct* PinStruct = Cast<UScriptStruct>(Pin->PinType.PinSubCategoryObject.Get());
						if(PinStruct && PinStruct->IsChildOf(FMassSharedFragment::StaticStruct()))
						{
							Pin_bConst->SafeSetHidden(false);
						}
						else
						{
							Pin_bConst->BreakAllPinLinks();
							Pin_bConst->SafeSetHidden(true);					
						}
					}
				}
			}
		}
		else
		{
			// link break
			Pin->PinType.PinCategory = UEdGraphSchema_K2::PC_Wildcard;
			Pin->PinType.PinSubCategory = NAME_None;
			Pin->PinType.PinSubCategoryObject = nullptr;
			Pin->PinType.bIsReference = true;
	
			Pin->BreakAllPinLinks();
			Pin_bConst->BreakAllPinLinks();
			Pin_bConst->SafeSetHidden(true);
		}
	}
	if(bAddDependency)
	{
		Pin_bConst->BreakAllPinLinks();
		Pin_bConst->SafeSetHidden(true);
	}
	GetGraph()->NotifyNodeChanged(this);
}

void UK2Node_GeneralAddToTrait::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	Super::ReallocatePinsDuringReconstruction(OldPins);
	Pin_bConst->SafeSetHidden(false);
}

void UK2Node_GeneralAddToTrait::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		auto CustomizeLambda = [](UEdGraphNode* NewNode, bool bIsTemplateNode, bool InbAddDependency)
		{
			UK2Node_GeneralAddToTrait* Node = CastChecked<UK2Node_GeneralAddToTrait>(NewNode);
			UFunction* Function = UMassScriptEntityTrait::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UMassScriptEntityTrait, AddToTrait));
			check(Function);
			Node->TargetFunction = Function;
			Node->SetIsAddDependency(InbAddDependency);
		};
		{
			UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
			check(NodeSpawner != nullptr);
			NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateLambda(CustomizeLambda,true);
			// NodeSpawner->DynamicUiSignatureGetter
			ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
		}
		{			
			UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
			check(NodeSpawner != nullptr);
			NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateLambda(CustomizeLambda,false);
			// NodeSpawner->DynamicUiSignatureGetter
			ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
		}
	}
}

FText UK2Node_GeneralAddToTrait::GetMenuCategory() const
{
	if(TargetFunction)
		return UK2Node_CallFunction::GetDefaultCategoryForFunction(TargetFunction, FText::GetEmpty());
	return Super::GetMenuCategory();
}

bool UK2Node_GeneralAddToTrait::IsActionFilteredOut(FBlueprintActionFilter const& Filter)
{
	return Super::IsActionFilteredOut(Filter);
}

bool UK2Node_GeneralAddToTrait::IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin,
														FString& OutReason) const
{
	bool CanConnect = Super::IsConnectionDisallowed(MyPin,OtherPin,OutReason);
	if(MyPin==GetStructPin() && !IsPinMassStruct(OtherPin))
	{
		CanConnect = true;
		OutReason = TEXT("Only Child Struct of MassFragment/MassTag/MassChunkFragment/MassSharedFragment can be connected");
	}
	return CanConnect;
	// const UEdGraphPin* ValuePin = FindPinChecked(FName(TEXT("Value")));
	//
	// if (MyPin == ValuePin && MyPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
	// {
	// 	if (OtherPin->PinType.PinCategory != UEdGraphSchema_K2::PC_Struct)
	// 	{
	// 		OutReason = TEXT("Value must be a struct.");
	// 		return true;
	// 	}
	// }
	//
	// return false;
}

void UK2Node_GeneralAddToTrait::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	Super::GetNodeContextMenuActions(Menu, Context);
}

void UK2Node_GeneralAddToTrait::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);
	auto Node = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this,SourceGraph);
	const auto Func = UMassScriptEntityTrait::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UMassScriptEntityTrait, AddToTrait));
	Node->SetFromFunction(Func);
	Node->AllocateDefaultPins();
	for(int i=0;i<Node->Pins.Num();i++)
	{
		if(this->Pins[i] == GetStructPin())
		{
			if(!IsPinMassStruct(GetStructPin()))
				continue;
		}
		CompilerContext.MovePinLinksToIntermediate(*this->Pins[i], *Node->Pins[i]);
	}
	Node->PostReconstructNode();
}

void UK2Node_GeneralAddToTrait::SetIsAddDependency(bool InbAddDependency)
{
	bAddDependency = InbAddDependency;
}

UEdGraphPin* UK2Node_GeneralAddToTrait::GetStructPin() const
{
	return Pins[3];
}

UStruct* UK2Node_GeneralAddToTrait::GetStruct() const
{
	return Cast<UStruct>(GetStructPin()->PinType.PinSubCategoryObject.Get());
}

bool UK2Node_GeneralAddToTrait::IsPinMassStruct(const UEdGraphPin* InPin)
{
	const auto PinStruct = Cast<UScriptStruct>(InPin->PinType.PinSubCategoryObject);
	if(!PinStruct)
		return false;
	
	if(PinStruct->IsChildOf(FMassFragment::StaticStruct())||
		PinStruct->IsChildOf(FMassTag::StaticStruct())||
		PinStruct->IsChildOf(FMassChunkFragment::StaticStruct())||
		PinStruct->IsChildOf(FMassSharedFragment::StaticStruct()))
	{
		return true;
	}	
	return false;
}

#undef LOCTEXT_NAMESPACE
