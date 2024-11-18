// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessingTypes.h"
#include "MassSettings.h"
#include "MassGameplayScriptSettings.generated.h"

#define GET_MASS_BP_CONFIG_VALUE(a) (GetMutableDefault<UMassGameplayScriptSettings>()->a)

class UMassScriptProcessor;
struct FPropertyChangedEvent;
/**
 * Implements the settings for MassBlueprint plugin
 */
UCLASS(config = MassGameplayScript, defaultconfig, DisplayName = "Mass Gameplay Script")
class MASSGAMEPLAYSCRIPT_API UMassGameplayScriptSettings : public UMassModuleSettings
{
	GENERATED_BODY()
public:
#if WITH_EDITORONLY_DATA
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnSettingsChange, const FPropertyChangedEvent& /*PropertyChangedEvent*/);
#endif // WITH_EDITORONLY_DATA
	DECLARE_MULTICAST_DELEGATE(FOnInitialized);

	UMassGameplayScriptSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	static FOnInitialized& GetOnInitializedEvent() { return GET_MASS_BP_CONFIG_VALUE(OnInitializedEvent); }
	void AddScriptProcessorClass(TSubclassOf<UMassScriptProcessor> Class);
	void RemoveScriptProcessorAsset(const FSoftClassPath& ClassPath);
	// void RemoveBlueprintProcessorClass();
#if WITH_EDITOR
	FOnSettingsChange& GetOnSettingsChange() { return OnSettingsChange; }

protected:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR
	virtual void BeginDestroy() override;

	void OnPostEngineInit();
public:
	/** This list contains all the processors available in the given binary (including plugins). The contents are sorted by display name.*/
	UPROPERTY(VisibleAnywhere, Category = Mass, Transient, EditFixedSize, meta=(MetaClass="/Script/MassGameplayScript.MassScriptProcessor"))
	TArray<FSoftClassPath> ScriptProcessorClasses;

#if WITH_EDITORONLY_DATA
protected:
	FOnSettingsChange OnSettingsChange;
#endif // WITH_EDITORONLY_DATA
	bool bEngineInitialized = false;

	FOnInitialized OnInitializedEvent;
};