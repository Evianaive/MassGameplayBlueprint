// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "K2Node_GetStaticVarRef.generated.h"

/**
 * 
 */
UCLASS()
class MASSGAMEPLAYSCRIPTKISMET_API UK2Node_GetStaticVarRef : public UK2Node
{
	GENERATED_BODY()
public:
	//~ Begin UEdGraphNode Interface._
	virtual void AllocateDefaultPins() override;
	virtual void PostReconstructNode() override;
	virtual bool IsNodePure() const override{ return true;}
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	//~ End UEdGraphNode Interface._

	
	virtual bool ShouldDrawCompact() const override { return true; }
	virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;	
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	bool bReturnByRefDesired = true;

	virtual FNodeHandlingFunctor* CreateNodeHandler(FKismetCompilerContext& CompilerContext) const override;

	UEdGraphPin* GetOutputPin() const;
};
