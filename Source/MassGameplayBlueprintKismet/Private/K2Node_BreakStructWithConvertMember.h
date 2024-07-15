// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_StructMemberGet.h"
#include "K2Node_BreakStructWithConvertMember.generated.h"

class FArchive;
class FBlueprintActionDatabaseRegistrar;
class FObjectPreSaveContext;
class UEdGraph;
class UEdGraphPin;
class UObject;
class UScriptStruct;

UCLASS(MinimalAPI)
class UK2Node_BreakStructWithConvertMember : public UK2Node_Variable
{
	GENERATED_UCLASS_BODY()

	/** Class that this variable is defined in.  */
	UPROPERTY()
	TObjectPtr<UScriptStruct> StructType;
	
	UPROPERTY(EditAnywhere, Category=PinOptions, EditFixedSize)
	TArray<FOptionalPinFromProperty> ShowPinForProperties;
	

protected:
	// Updater for subclasses that allow hiding pins
	struct FStructOperationOptionalPinManager : public FOptionalPinManager
	{
		//~ Begin FOptionalPinsUpdater Interface
		virtual void GetRecordDefaults(FProperty* TestProperty, FOptionalPinFromProperty& Record) const override
		{
			Record.bCanToggleVisibility = true;
			Record.bShowPin = true;
			if (TestProperty)
			{
				Record.bShowPin = !TestProperty->HasMetaData(TEXT("PinHiddenByDefault"));
				if (Record.bShowPin)
				{
					if (UStruct* OwnerStruct = TestProperty->GetOwnerStruct())
					{
						Record.bShowPin = !OwnerStruct->HasMetaData(TEXT("HiddenByDefault"));
					}
				}
			}
		}

		virtual void CustomizePinData(UEdGraphPin* Pin, FName SourcePropertyName, int32 ArrayIndex, FProperty* Property) const override;
		// End of FOptionalPinsUpdater interfac
	};
	
	bool DoRenamedPinsMatch(const UEdGraphPin* NewPin, const UEdGraphPin* OldPin, bool bStructInVariablesOut) const;
public:
	/** Helper property to handle upgrades from an old system of displaying pins for
	*	the override values that properties referenced as a conditional of being set in a struct */
	UPROPERTY()
	bool bMadeAfterOverridePinRemoval;

	/** 
	 * Returns false if:
	 *   1. The Struct has a 'native break' method
	 * Returns true if:
	 *   1. The Struct is tagged as BlueprintType
	 *   and
	 *   2. The Struct has any property that is tagged as CPF_BlueprintVisible
	 */
	static bool CanBeBroken(const UScriptStruct* Struct, bool bForInternalUse = false);

	/** Can this struct be used as a split pin */
	static bool CanBeSplit(const UScriptStruct* Struct) { return CanBeBroken(Struct); }

	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual void ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const override;
	virtual void PostPlacedNewNode() override;
	//~ End  UEdGraphNode Interface

	//~ Begin K2Node Interface
	virtual bool NodeCausesStructuralBlueprintChange() const override { return false; }
	virtual bool IsNodePure() const override { return true; }
	virtual bool DrawNodeAsVariable() const override { return false; }
	virtual ERedirectType DoPinsMatchForReconstruction(const UEdGraphPin* NewPin, int32 NewPinIndex, const UEdGraphPin* OldPin, int32 OldPinIndex) const override;
	virtual class FNodeHandlingFunctor* CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	//~ End K2Node Interface

private:
	/** Constructing FText strings can be costly, so we cache the node's title/tooltip */
	FNodeTextCache CachedTooltip;
	FNodeTextCache CachedNodeTitle;
};

