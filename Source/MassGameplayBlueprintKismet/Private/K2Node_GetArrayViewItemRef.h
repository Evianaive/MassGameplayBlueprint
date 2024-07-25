// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "K2Node_GetArrayViewItemRef.generated.h"

/**
 * 
 */
UCLASS()
class MASSGAMEPLAYSCRIPTKISMET_API UK2Node_GetArrayViewItemRef : public UK2Node
{
	GENERATED_BODY()
public:
	//~ Begin UEdGraphNode Interface._
	virtual void AllocateDefaultPins() override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual void PostReconstructNode() override;
	virtual bool IsNodePure() const override{ return true;}
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	// Center Image from output pin, When Draw as compact node
	virtual TSharedPtr<SWidget> CreateNodeImage() const override;
	//~ End UEdGraphNode Interface._

	//~ Begin UK2Node Interface_
	// Draw as compact node like ArrayGetByRef
	virtual bool ShouldDrawCompact() const override { return true; }
	
	// when link pin
	virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;	
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	
	virtual int32 GetNodeRefreshPriority() const override { return EBaseNodeRefreshPriority::Low_UsesDependentWildcard; }

	virtual FText GetMenuCategory() const override;
	
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual bool IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const override;
	virtual void PreloadRequiredAssets() override;
	//~ End UK2Node Interface_
	bool bReturnByRefDesired = true;

	/* used to fool blueprint compiler*/
	UEdGraphPin* FakeArrayInputPin;

	UEdGraphPin* GetOutputPin() const {return FindPinChecked(Output,EGPD_Output);};
	UEdGraphPin* GetIndexPin() const {return FindPinByPredicate([](const UEdGraphPin* InPin)
	{
		return InPin->PinType.PinCategory==UEdGraphSchema_K2::PC_Int;
	});};
	UEdGraphPin* GetArrayViewPin() const {return FindPinByPredicate([](const UEdGraphPin* InPin)
	{
		return InPin->PinType.PinCategory==UEdGraphSchema_K2::PC_Struct;
	});};
	static inline FName Output = TEXT("Output");
};
