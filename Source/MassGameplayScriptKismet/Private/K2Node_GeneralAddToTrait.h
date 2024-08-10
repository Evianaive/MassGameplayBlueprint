// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "K2Node_GeneralAddToTrait.generated.h"

/**
 * 
 */
UCLASS()
class MASSGAMEPLAYSCRIPTKISMET_API UK2Node_GeneralAddToTrait : public UK2Node
{
	GENERATED_BODY()
public:
	//~ Begin UEdGraphNode Interface._
	virtual void AllocateDefaultPins() override;
	virtual void PostReconstructNode() override;
	virtual bool IsNodePure() const override{ return false;}
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	//~ End UEdGraphNode Interface._
	
	virtual bool ShouldDrawCompact() const override { return false; }
	virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;	
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;
	virtual bool IsActionFilteredOut(FBlueprintActionFilter const& Filter) override;
	virtual bool IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const override;
	virtual void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;

	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	void SetIsAddDependency(bool InbAddDependency);
	
	UFunction* TargetFunction;
	bool bAddDependency;
	UEdGraphPin* Pin_bConst;
	
	UEdGraphPin* GetStructPin() const;
	UStruct* GetStruct() const;
};
