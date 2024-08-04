// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintClass/MassScriptEntityTrait.h"
#include "MassEntityTemplateRegistry.h"
#include "MassEntityTypes.h"

#include "Utility/PrivateAccessor.hpp"

DECLARE_PRIVATE_ACCESS(FMassEntityTemplateData,Composition,FMassArchetypeCompositionDescriptor)
struct FLocalDecorator : public FMassEntityTemplateBuildContext
{
	void AddChunkFragment(const UScriptStruct& Struct)
	{
		TypeAdded(Struct);		
		(TemplateData.*PRIVATE_ACCESS(FMassEntityTemplateData,Composition)).ChunkFragments.Add(Struct);
	}
};
#define MAP_AB TMap<uint32, int32>
DECLARE_PRIVATE_ACCESS(FMassEntityManager,SharedFragmentsMap,MAP_AB);
DECLARE_PRIVATE_ACCESS(FMassEntityManager,ConstSharedFragmentsMap,MAP_AB);
#define MAP_AB TMap<UScriptStruct*, TArray<FSharedStruct>>
DECLARE_PRIVATE_ACCESS(FMassEntityManager,SharedFragmentsTypeMap,MAP_AB)
#undef MAP_AB
DECLARE_PRIVATE_ACCESS(FMassEntityManager,SharedFragments,TArray<FSharedStruct>)
DECLARE_PRIVATE_ACCESS(FMassEntityManager,ConstSharedFragments,TArray<FConstSharedStruct>)

void UMassScriptEntityTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	const_cast<UMassScriptEntityTrait*>(this)->TempBuildContext = reinterpret_cast<FLocalDecorator*>(&BuildContext);
	const_cast<UMassScriptEntityTrait*>(this)->TempWorld = &World;
	BuildTemplateBP();
	const_cast<UMassScriptEntityTrait*>(this)->TempBuildContext = nullptr;
	const_cast<UMassScriptEntityTrait*>(this)->TempWorld = nullptr;
}

void UMassScriptEntityTrait::BuildTemplateBP_Implementation() const
{
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(*TempWorld);
	for (const auto Property :TFieldRange<FStructProperty>(this->GetClass()))
	{
		const FString& PropertyCategory = Property->GetMetaData(TEXT("Category"));
		// Todo FMassChunkFragment
		if(Property->Struct->IsChildOf(FMassFragment::StaticStruct()))
		{
			TempBuildContext->AddFragment(FConstStructView(Property->Struct,Property->ContainerPtrToValuePtr<uint8>(this)));
			continue;
		}
		if(Property->Struct->IsChildOf(FMassChunkFragment::StaticStruct()))
		{
			TempBuildContext->AddChunkFragment(*Property->Struct);
			continue;
		}
		if(Property->Struct->IsChildOf(FMassSharedFragment::StaticStruct()))
		{
			uint8 const* PropertyValue = Property->ContainerPtrToValuePtr<uint8>(this);
			uint32 Hash = UE::StructUtils::GetStructCrc32(FConstStructView(Property->Struct,PropertyValue));
			if(PropertyCategory == TEXT("SharedFragments"))
			{
				int32& Index = (EntityManager.*PRIVATE_ACCESS(FMassEntityManager,SharedFragmentsMap)).FindOrAddByHash(Hash, Hash, INDEX_NONE);
				auto& SharedFragmentsRef = EntityManager.*PRIVATE_ACCESS(FMassEntityManager,SharedFragments);
				if (Index == INDEX_NONE)
				{
					Index = (EntityManager.*PRIVATE_ACCESS(FMassEntityManager,SharedFragments)).Add(FSharedStruct::Make(Property->Struct,PropertyValue));
					// note that even though we're copying the freshly created FSharedStruct instance it's perfectly fine since 
					// FSharedStruct do guarantee there's not going to be data duplication (via a member shared pointer to hosted data)
					TArray<FSharedStruct>& InstancesOfType = (EntityManager.*PRIVATE_ACCESS(FMassEntityManager,SharedFragmentsTypeMap)).FindOrAdd(Property->Struct, {});
					InstancesOfType.Add(SharedFragmentsRef[Index]);
				}
				TempBuildContext->AddSharedFragment(SharedFragmentsRef[Index]);
				continue;
			}
			if(PropertyCategory == TEXT("ConstSharedFragments"))
			{
				int32& Index = (EntityManager.*PRIVATE_ACCESS(FMassEntityManager,ConstSharedFragmentsMap)).FindOrAddByHash(Hash, Hash, INDEX_NONE);
				auto& ConstSharedFragmentsRef = EntityManager.*PRIVATE_ACCESS(FMassEntityManager,ConstSharedFragments);
				if (Index == INDEX_NONE)
				{
					Index = ConstSharedFragmentsRef.Add(FSharedStruct::Make(Property->Struct,PropertyValue));
				}
				TempBuildContext->AddConstSharedFragment(ConstSharedFragmentsRef[Index]);
				continue;
			}
		}
		if(Property->Struct->IsChildOf(FMassTag::StaticStruct()))
		{
			TempBuildContext->AddTag(*Property->Struct);
			continue;
		}
	}
}

void UMassScriptEntityTrait::DestroyTemplate() const
{
	Super::DestroyTemplate();
}

void UMassScriptEntityTrait::ValidateTemplate(FMassEntityTemplateBuildContext& BuildContext,
												const UWorld& World) const
{
	Super::ValidateTemplate(BuildContext, World);
}
