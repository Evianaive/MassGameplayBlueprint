#pragma once
#include "AssetRegistry/IAssetRegistry.h"

class FMassProcessorBlueprintAutoManager
{
public:
	FMassProcessorBlueprintAutoManager();
	
	/** Callback registered to the Asset Registry to be notified when an asset is added. */
	void AddAsset(const FAssetData& InAddedAssetData);
	/** Callback registered to the Asset Registry to be notified when an asset is removed. */
	void RemoveAsset(const FAssetData& InRemovedAssetData);
	/** Called when reload has finished */
	void OnReloadComplete(EReloadCompleteReason ReloadCompleteReason);

	static void OnModulesChanged(FName Name, EModuleChangeReason ModuleChangeReason);
	
	static void UpdateBlueprintProcessor();
	FDelegateHandle OnFilesLoadedUpdateBlueprintProcessorHandle;
	FDelegateHandle OnBlueprintCompiledUpdateBlueprintProcessorHandle;
	FDelegateHandle OnClassPackageLoadedOrUnloadedUpdateBlueprintProcessorHandle;
};
