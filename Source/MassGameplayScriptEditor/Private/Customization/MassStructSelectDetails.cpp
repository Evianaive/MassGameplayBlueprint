#include "Customization/MassStructSelectDetails.h"

#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IPropertyUtilities.h"
#include "StructViewerFilter.h"
#include "StructViewerModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/UserDefinedStruct.h"
#include "Styling/SlateIconFinder.h"
#include "UserDefinedStructInherit/MassBasicStructFilter.h"
#include "UserDefinedStructInherit/StructInheritExtender.h"

class FStructFilter_Internal : public IStructViewerFilter
{
public:
	/** The meta struct for the property that classes must be a child-of. */
	const UScriptStruct* MetaStruct = nullptr;
	virtual bool IsStructAllowed(const FStructViewerInitializationOptions& InInitOptions, const UScriptStruct* InStruct, TSharedRef<FStructViewerFilterFuncs> InFilterFuncs) override
	{
		// Query the native struct to see if it has the correct parent type (if any)
		return !MetaStruct || InStruct->IsChildOf(MetaStruct);
	}

	/**
	 * In order to judge whether the user defined struct is child of FMassFragment, we need to add tags in their asset data!
	 * Implement in FMassStructSelectExtenderCreator
	 */
	virtual bool IsUnloadedStructAllowed(const FStructViewerInitializationOptions& InInitOptions, const FSoftObjectPath& InStructPath, TSharedRef<FStructViewerFilterFuncs> InFilterFuncs) override
	{
		if(!MetaStruct)
			return true;
		// User Defined Structs don't support inheritance, so only include them if we have don't a MetaStruct set
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(InStructPath);
		FSoftObjectPath SuperStructString{AssetData.GetTagValueRef<FString>(FStructEditorExtenderCreator_MassSelect::SuperStructMetaData)};
		UScriptStruct* SuperStruct = Cast<UScriptStruct>(SuperStructString.TryLoad());
		if(SuperStruct && SuperStruct->IsChildOf(MetaStruct))
			return true;
		
		return false;
	}
};

TSharedRef<IPropertyTypeCustomization> FMassStructSelectDetails::MakeInstance()
{	
	return MakeShared<FMassStructSelectDetails>();
}

void FMassStructSelectDetails::CustomizeHeader(
	TSharedRef<IPropertyHandle> PropertyHandle,
	FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	TSharedPtr<IPropertyUtilities> PropUtils = CustomizationUtils.GetPropertyUtilities();
	
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
			.Text_Lambda([PropertyHandle]()
			{
				void* TempStruct;
				PropertyHandle->GetValueData(TempStruct);
				UScriptStruct* Struct = *static_cast<UScriptStruct**>(TempStruct);
				if(Struct)
					return Struct->GetDisplayNameText();
				return FText::FromString(TEXT("None"));
			})
			.ToolTipText(FText::FromString(TEXT("Super struct")))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		]
	];
	
	ComboButton->SetOnGetMenuContent(FOnGetContent::CreateLambda([ComboButton, PropertyHandle, PropUtils]()
	{
		FStructViewerInitializationOptions Options;
		Options.Mode = EStructViewerMode::StructPicker;
		Options.bShowNoneOption = true;
		Options.DisplayMode = EStructViewerDisplayMode::TreeView;
		
		
		TSharedPtr<FStructFilter_Internal> Filter =  MakeShared<FStructFilter_Internal>();
		FString MetaStructString = PropertyHandle->GetProperty()->GetMetaData(TEXT("MetaStruct"));
		if(UScriptStruct* MetaStruct = LoadObject<UScriptStruct>(nullptr,*MetaStructString))
		{
			Filter->MetaStruct = MetaStruct;
		}
		Options.StructFilter = Filter;
		void* StructType;
		PropertyHandle->GetValueData(StructType);
		Options.SelectedStruct = static_cast<UScriptStruct*>(StructType);
		
		FStructViewerModule& StructViewerModule = FModuleManager::LoadModuleChecked<FStructViewerModule>("StructViewer");
		return StructViewerModule.CreateStructViewer(Options,FOnStructPicked::CreateLambda(
			[PropertyHandle, ComboButton, PropUtils](const UScriptStruct* InStruct)
			{
				if (PropertyHandle->IsValidHandle())
				{
					FScopedTransaction Transaction(NSLOCTEXT( "MassStructSelect","OnStructPicked", "Set Struct"));
					PropertyHandle->NotifyPreChange();
					PropertyHandle->EnumerateRawData([InStruct](void* RawData, const int32 /*DataIndex*/, const int32 /*NumDatas*/)
					{
						*static_cast<const UScriptStruct**>(RawData) = InStruct;
						return true;
					});
					PropertyHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
					PropertyHandle->NotifyFinishedChangingProperties();
					// Property tree will be invalid after changing the struct type, force update.
					if (PropUtils.IsValid())
					{
						PropUtils->ForceRefresh();
					}
				}
				ComboButton->SetIsOpen(false);
			}
		));
	}));
	HeaderRow
		.NameContent()
		[
			PropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MinDesiredWidth(250.f)
		.VAlign(VAlign_Center)
		[
			ComboButton
		];
}

void FMassStructSelectDetails::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}
