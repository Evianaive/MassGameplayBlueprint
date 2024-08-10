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
	bool HasTag(const UScriptStruct& Struct) const
	{
		return TemplateData.GetCompositionDescriptor().Tags.Contains(Struct);
	}
	bool HasChunkFragment(const UScriptStruct& Struct) const
	{
		return TemplateData.GetCompositionDescriptor().ChunkFragments.Contains(Struct);
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
	const_cast<UMassScriptEntityTrait*>(this)->BuildTemplateBP();
	const_cast<UMassScriptEntityTrait*>(this)->TempBuildContext = nullptr;
	const_cast<UMassScriptEntityTrait*>(this)->TempWorld = nullptr;
}

void UMassScriptEntityTrait::BuildTemplateBP_Implementation()
{
	for (const auto Property :TFieldRange<FStructProperty>(this->GetClass()))
	{
		const FString& PropertyCategory = Property->GetMetaData(TEXT("Category"));
		bool bSharedFragmentIsConst = false;
		if(Property->Struct->IsChildOf(FMassSharedFragment::StaticStruct()))
		{
			bSharedFragmentIsConst = (PropertyCategory == TEXT("ConstSharedFragments"));
			// Stop if SharedFragment Property is not in these two category
			if(!bSharedFragmentIsConst && (PropertyCategory != TEXT("SharedFragments")))
			{
				UE_LOG(LogTemp,Log,TEXT("Can't determine which SharedFragments to add to"));
				continue;
			}
		}
		uint8 const* PropertyValue = Property->ContainerPtrToValuePtr<uint8>(this);
		Generic_AddToTrait(Property,PropertyValue,bSharedFragmentIsConst);
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

bool UMassScriptEntityTrait::HasTraitContains(const int32& Structure)
{
	checkNoEntry()
	return false;
}

DEFINE_FUNCTION(UMassScriptEntityTrait::execHasTraitContains)
{
	Stack.MostRecentProperty = nullptr;
	Stack.MostRecentPropertyAddress = nullptr;	
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* StructAddr = Stack.MostRecentPropertyAddress;
	FStructProperty* StructProperty = CastField<FStructProperty>(Stack.MostRecentProperty);
	
	P_FINISH;
	P_NATIVE_BEGIN;
	
	*(bool*)RESULT_PARAM = false;
	if(StructProperty->Struct!=nullptr)
	{
		if(StructProperty->Struct->IsChildOf(FMassFragment::StaticStruct()))
			*(bool*)RESULT_PARAM = P_THIS->TempBuildContext->HasFragment(*StructProperty->Struct);
		else if(StructProperty->Struct->IsChildOf(FMassTag::StaticStruct()))
			*(bool*)RESULT_PARAM = P_THIS->TempBuildContext->HasTag(*StructProperty->Struct);
		else if(StructProperty->Struct->IsChildOf(FMassChunkFragment::StaticStruct()))
			*(bool*)RESULT_PARAM = P_THIS->TempBuildContext->HasChunkFragment(*StructProperty->Struct);	
		else if(StructProperty->Struct->IsChildOf(FMassSharedFragment::StaticStruct()))
			*(bool*)RESULT_PARAM = P_THIS->TempBuildContext->HasSharedFragment(*StructProperty->Struct);
		else
			*(bool*)RESULT_PARAM = false;
	}
	P_NATIVE_END;
}

void UMassScriptEntityTrait::AddToTrait(const int32& Structure, bool bAddDependency, bool bConst)
{
	checkNoEntry()
}

DEFINE_FUNCTION(UMassScriptEntityTrait::execAddToTrait)
{
	Stack.MostRecentProperty = nullptr;
	Stack.MostRecentPropertyAddress = nullptr;	
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* StructAddr = Stack.MostRecentPropertyAddress;
	FStructProperty* StructProperty = CastField<FStructProperty>(Stack.MostRecentProperty);
	P_GET_UBOOL(bAddDependency);
	P_GET_UBOOL(bConst);
	
	P_FINISH;
	P_NATIVE_BEGIN;
	if(bAddDependency && StructProperty->Struct)
	{
		P_THIS->TempBuildContext->AddDependency(StructProperty->Struct);
	}
	else
	{
		P_THIS->Generic_AddToTrait(StructProperty,static_cast<uint8 const*>(StructAddr),bConst);
	}
	P_NATIVE_END;
}

void UMassScriptEntityTrait::Generic_AddToTrait(const FStructProperty* Property, uint8 const* PropertyValue, bool IsConst) const
{
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(*TempWorld);
	if(Property->Struct->IsChildOf(FMassFragment::StaticStruct()))
	{
		TempBuildContext->AddFragment(FConstStructView(Property->Struct,PropertyValue));
		// UE_LOG(LogTemp,Log,TEXT("AddFragment"))
		return;
	}
	if(Property->Struct->IsChildOf(FMassChunkFragment::StaticStruct()))
	{
		TempBuildContext->AddChunkFragment(*Property->Struct);
		// UE_LOG(LogTemp,Log,TEXT("AddChunkFragment"))
		return;
	}
	if(Property->Struct->IsChildOf(FMassSharedFragment::StaticStruct()))
	{
		uint32 Hash = UE::StructUtils::GetStructCrc32(FConstStructView(Property->Struct,PropertyValue));
		if(IsConst)
		{
			int32& Index = (EntityManager.*PRIVATE_ACCESS(FMassEntityManager,ConstSharedFragmentsMap)).FindOrAddByHash(Hash, Hash, INDEX_NONE);
			auto& ConstSharedFragmentsRef = EntityManager.*PRIVATE_ACCESS(FMassEntityManager,ConstSharedFragments);
			if (Index == INDEX_NONE)
			{
				Index = ConstSharedFragmentsRef.Add(FSharedStruct::Make(Property->Struct,PropertyValue));
			}
			TempBuildContext->AddConstSharedFragment(ConstSharedFragmentsRef[Index]);
			// UE_LOG(LogTemp,Log,TEXT("AddConstSharedFragment"))
			return;
		}
		else
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
			// UE_LOG(LogTemp,Log,TEXT("AddSharedFragment"))
			return;
		}
	}
	if(Property->Struct->IsChildOf(FMassTag::StaticStruct()))
	{
		TempBuildContext->AddTag(*Property->Struct);
		// UE_LOG(LogTemp,Log,TEXT("AddTag"))
		return;
	}
}