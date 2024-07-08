// Fill out your copyright notice in the Description page of Project Settings.


#include "MassEntityTraitBlueprint.h"

#include "MassEntityTemplateRegistry.h"
#include "MassEntityTypes.h"

void UMassEntityTraitBlueprint::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	for (const auto Property :TFieldRange<FStructProperty>(this->GetClass()))
	{
		if(Property->Struct->IsChildOf(FMassFragment::StaticStruct()))
		{
			BuildContext.AddFragment(FConstStructView(Property->Struct,Property->ContainerPtrToValuePtr<uint8>(this)));
			return;
		}
		if(Property->Struct->IsChildOf(FMassTag::StaticStruct()))
		{
			BuildContext.AddTag(*Property->Struct);
			return;
		}
	}	
}

void UMassEntityTraitBlueprint::DestroyTemplate() const
{
	Super::DestroyTemplate();
}

void UMassEntityTraitBlueprint::ValidateTemplate(FMassEntityTemplateBuildContext& BuildContext,
												const UWorld& World) const
{
	Super::ValidateTemplate(BuildContext, World);
}
