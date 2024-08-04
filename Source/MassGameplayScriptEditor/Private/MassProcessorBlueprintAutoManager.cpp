#include "MassProcessorBlueprintAutoManager.h"

#include "EditorClassUtils.h"
#include "MassGameplayScriptSettings.h"
#include "BlueprintClass/MassScriptProcessor.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Editor/ClassViewer/Private/ClassViewerNode.h"
#include "UObject/CoreRedirects.h"

FMassProcessorBlueprintAutoManager::FMassProcessorBlueprintAutoManager()
{
	// Register with the Asset Registry to be informed when it is done loading up files.
	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName).Get();
	OnFilesLoadedUpdateBlueprintProcessorHandle = AssetRegistry.OnFilesLoaded().AddStatic( &FMassProcessorBlueprintAutoManager::UpdateBlueprintProcessor );
	AssetRegistry.OnAssetAdded().AddRaw( this, &FMassProcessorBlueprintAutoManager::AddAsset);
	AssetRegistry.OnAssetRemoved().AddRaw( this, &FMassProcessorBlueprintAutoManager::RemoveAsset );

	// Register to have Populate called when doing a Reload.
	FCoreUObjectDelegates::ReloadCompleteDelegate.AddRaw( this, &FMassProcessorBlueprintAutoManager::OnReloadComplete );

	// Register to have Populate called when a Blueprint is compiled.
	
	// OnBlueprintCompiledUpdateBlueprintProcessorHandle            = GEditor->OnBlueprintCompiled().AddStatic( &FMassProcessorBlueprintAutoManager::UpdateBlueprintProcessor );
	// OnClassPackageLoadedOrUnloadedUpdateBlueprintProcessorHandle = GEditor->OnClassPackageLoadedOrUnloaded().AddStatic( &FMassProcessorBlueprintAutoManager::UpdateBlueprintProcessor );

	FModuleManager::Get().OnModulesChanged().AddStatic( &FMassProcessorBlueprintAutoManager::OnModulesChanged);

	// FMassProcessorBlueprintAutoManager::UpdateBlueprintProcessor();
}

void FixupClassCoreRedirects(FTopLevelAssetPath& InOutClassPath)
{
	const FCoreRedirectObjectName OldName = FCoreRedirectObjectName(InOutClassPath);
	const FCoreRedirectObjectName NewName = FCoreRedirects::GetRedirectedName(ECoreRedirectFlags::Type_Class, OldName);
	
	if (OldName != NewName)
	{
		const FString OldClassPath = InOutClassPath.ToString();

		// Only do the fixup if the old class name isn't in memory.
		const UClass* FoundOldClass = UClass::TryFindTypeSlow<UClass>(OldClassPath);
		const FTopLevelAssetPath NewClassPath(NewName.ToString());

		if (!FoundOldClass || FoundOldClass->GetClassPathName() == NewClassPath)
		{
			InOutClassPath = NewClassPath;
		}
	}
};

void FMassProcessorBlueprintAutoManager::AddAsset(const FAssetData& InAddedAssetData)
{
	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName).Get();
	if (AssetRegistry.IsLoadingAssets())
	{
		return;
	}
	FTopLevelAssetPath ClassPath = FEditorClassUtils::GetClassPathNameFromAsset(InAddedAssetData);
	
	if(!ClassPath.IsNull())
	{
		FixupClassCoreRedirects(ClassPath);
		if(UClass* Class = FindObject<UClass>(nullptr, *ClassPath.ToString()))
		{
			if(Class->IsChildOf(UMassScriptProcessor::StaticClass()))
				GetMutableDefault<UMassGameplayScriptSettings>()->AddBlueprintProcessorClass(Class);			
		}		
		// TSharedPtr<FClassViewerNode>& Node = ClassPathToNode.FindOrAdd(ClassPath);
		// InAddedAssetData.GetSoftObjectPath()

		// ClassPath.ass
		// CreateOrUpdateUnloadedClassNode(Node, AssetData, ClassPath);
	}
}

void FMassProcessorBlueprintAutoManager::RemoveAsset(const FAssetData& InRemovedAssetData)
{
	// BPGCs can be missing if it was already deleted prior to the notification being sent. 
	// Let's try to reconstruct the generated class path from the BP object path.
	bool bGenerateClassPathIfMissing = true;
	const FTopLevelAssetPath ClassPath = FEditorClassUtils::GetClassPathNameFromAsset(InRemovedAssetData, bGenerateClassPathIfMissing);

	if(!ClassPath.IsNull())
	{
		GetMutableDefault<UMassGameplayScriptSettings>()->RemoveBlueprintProcessorAsset(ClassPath.ToString());
	}
}

void FMassProcessorBlueprintAutoManager::OnReloadComplete(EReloadCompleteReason ReloadCompleteReason)
{
}

void FMassProcessorBlueprintAutoManager::OnModulesChanged(FName Name, EModuleChangeReason ModuleChangeReason)
{
}

void FMassProcessorBlueprintAutoManager::UpdateBlueprintProcessor()
{
	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName).Get();
	FString ClassPathString;

	TArray<FAssetData> Assets;

	AssetRegistry.GetAssetsByClass(UBlueprint::StaticClass()->GetClassPathName(), Assets, /*bSearchSubClasses=*/true);
	const auto& MassProcessorBlueprintClassPath = UMassScriptProcessor::StaticClass()->GetClassPathName();
	
	for (const FAssetData& AssetData : Assets)
	{
		FTopLevelAssetPath ClassPath = FEditorClassUtils::GetClassPathNameFromAssetTag(AssetData);
		if (ClassPath.IsNull())
		{
			// 		UE_LOG(LogEditorClassViewer, Warning, TEXT("AssetRegistry Blueprint %s is missing tag value for %s. Blueprint will not be available to ClassViewer when unloaded."),
			// 			*AssetData.GetObjectPathString(), *FBlueprintTags::GeneratedClassPath.ToString());
			continue;
		}
		FixupClassCoreRedirects(ClassPath);
		//此处应该还是不需要Load，因为ClassPicker也没load，但能选蓝图子类
		FString ParentClassPathString;
		if(AssetData.GetTagValue(FBlueprintTags::ParentClassPath,ParentClassPathString))
		{
			FTopLevelAssetPath ParentClassPath(FTopLevelAssetPath(*FPackageName::ExportTextPathToObjectPath(ParentClassPathString)));
			FixupClassCoreRedirects(ParentClassPath);
			
			const bool bNormalBlueprintType = AssetData.GetTagValueRef<FString>(FBlueprintTags::BlueprintType) == TEXT("BPType_Normal");
			const uint32 ClassFlags = AssetData.GetTagValueRef<uint32>(FBlueprintTags::ClassFlags);
			
			FSoftClassPath NativeClassPath(AssetData.GetTagValueRef<FString>(FBlueprintTags::NativeParentClassPath));
			if(MassProcessorBlueprintClassPath != NativeClassPath.GetAssetPath())
				continue;
			
			if(const auto Class = LoadObject<UClass>(nullptr, *ClassPath.ToString()))
			{
				GetMutableDefault<UMassGameplayScriptSettings>()->AddBlueprintProcessorClass(Class);	
			}	
		}
	}
	
	Assets.Reset();
	AssetRegistry.GetAssetsByClass(UBlueprintGeneratedClass::StaticClass()->GetClassPathName(), Assets, /*bSearchSubClasses=*/true);

	for (const FAssetData& AssetData : Assets)
	{
		FTopLevelAssetPath ClassPathNameFromAssetPath = AssetData.GetSoftObjectPath().GetAssetPath();
		
		if(!ClassPathNameFromAssetPath.IsNull())
		{
			FixupClassCoreRedirects(ClassPathNameFromAssetPath);
			if(UClass* Class = FindObject<UClass>(nullptr, *ClassPathNameFromAssetPath.ToString()))
			{
				GetMutableDefault<UMassGameplayScriptSettings>()->AddBlueprintProcessorClass(Class);			
			}
		}
	}
}
