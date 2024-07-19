// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_GetArrayViewItemRef.h"
#include "K2Node_GetArrayViewItemRefCode.generated.h"

/**
 * 
 */
UCLASS()
class MASSGAMEPLAYBLUEPRINTKISMET_API UK2Node_GetArrayViewItemRefCode : public UK2Node_GetArrayViewItemRef
{
	GENERATED_BODY()
public:
	//~ Begin UEdGraphNode Interface._
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual FNodeHandlingFunctor* CreateNodeHandler(FKismetCompilerContext& CompilerContext) const override;
	//~ End UEdGraphNode Interface._
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
};
