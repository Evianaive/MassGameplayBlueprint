﻿#include "MassStructFilter.h"

#include "DetailLayoutBuilder.h"
#include "StructViewerModule.h"
#include "Engine/UserDefinedStruct.h"
#include "Styling/SlateIconFinder.h"

bool MassStructFilter::IsStructAllowed(const FStructViewerInitializationOptions& InInitOptions,
										const UScriptStruct* InStruct, TSharedRef<FStructViewerFilterFuncs> InFilterFuncs)
{
	if(InStruct->GetStructureSize()!=1)
		return false;	
	static FName ModulePath = "/Script/MassEntity";
	if(!InStruct->GetName().Contains("Mass"))
		return false;	
	if(InStruct->GetStructPathName().GetPackageName()!=ModulePath)
		return false;
	
	return true;
}

bool MassStructFilter::IsUnloadedStructAllowed(const FStructViewerInitializationOptions& InInitOptions,
	const FSoftObjectPath& InStructPath, TSharedRef<FStructViewerFilterFuncs> InFilterFuncs)
{
	return false;
}

void FMassStructSelectExtenderCreator::OnUserDefinedStructEditorOpen(UObject* Object, IAssetEditorInstance* EditorInstance)
{
	UUserDefinedStruct* EditStruct = Cast<UUserDefinedStruct>(Object);
	if(!EditStruct)
		return;
	
	FAssetEditorToolkit* AssetEditorToolkit = StaticCast<FAssetEditorToolkit*>(EditorInstance);

	TSharedPtr<FExtender> Extender = MakeShared<FExtender>();
	Extender->AddToolBarExtension("Asset", EExtensionHook::After, AssetEditorToolkit->GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateLambda([=](FToolBarBuilder& ToolbarBuilder)
		{			
			auto ComboButton = SNew(SComboButton)
			.ContentPadding(0)
			.ButtonContent()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(0.0f, 0.0f, 4.0f, 0.0f)
				[
					SNew(SImage)
					.Image(FSlateIconFinder::FindIconBrushForClass(UScriptStruct::StaticClass()))
				]
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text_Lambda([EditStruct]()
					{
						if(auto EditStructSuper = EditStruct->GetSuperStruct())						
							return EditStructSuper->GetDisplayNameText();
						return FText::FromString(TEXT("No Parent"));
					})
					.ToolTipText(FText::FromString(TEXT("Super struct")))
					.Font(IDetailLayoutBuilder::GetDetailFont())
				]
			];
			ComboButton->SetOnGetMenuContent(FOnGetContent::CreateLambda([EditStruct, ComboButton]()
			{
				FStructViewerInitializationOptions Options;
				Options.Mode = EStructViewerMode::StructPicker;
				Options.bShowNoneOption = true;
				Options.StructFilter =  MakeShared<MassStructFilter>();
				Options.DisplayMode = EStructViewerDisplayMode::TreeView;
				Options.SelectedStruct = reinterpret_cast<UScriptStruct*>(EditStruct->GetSuperStruct());
				
				FStructViewerModule& StructViewerModule = FModuleManager::LoadModuleChecked<FStructViewerModule>("StructViewer");
				return StructViewerModule.CreateStructViewer(Options,FOnStructPicked::CreateLambda([EditStruct, ComboButton](const UScriptStruct* Struct)
				{
					// We don't need to clear member for tag because tag will not allocate
					
					// if(Struct == TBaseStructure<FMassTag>::Get())
					// if(Struct->GetFName()==TEXT("MassTag"))
					// {
					// 	UUserDefinedStructEditorData* DuplicatedEditorData = CastChecked<UUserDefinedStructEditorData>(EditStruct->EditorData);
					// 	DuplicatedEditorData->VariablesDescriptions.Reset();
					// 	FStructureEditorUtils::OnStructureChanged(EditStruct,FStructureEditorUtils::AddedVariable);
					// 	// DuplicatedEditorData->RecreateDefaultInstance();
					// }
					EditStruct->SetSuperStruct(const_cast<UScriptStruct*>(Struct));
					EditStruct->Modify();
					ComboButton->SetIsOpen(false);
				}));
			}));
			
			ToolbarBuilder.AddWidget(
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(0,0,5,0)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Parent Struct")))
					.Font(IDetailLayoutBuilder::GetDetailFont())
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					ComboButton
				]);
		}));
	AssetEditorToolkit->AddToolbarExtender(Extender);
}
