// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/BuildingRegistry.h"
#include "Buildings/BuildingDefinition.h"
#include "Engine/AssetManager.h"

void UBuildingRegistry::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	RequestDefinitionsLoad();
}

void UBuildingRegistry::Deinitialize()
{
	Definitions.Empty();
	Super::Deinitialize();
}

void UBuildingRegistry::RequestDefinitionsLoad()
{
	UAssetManager& AssetManager = UAssetManager::Get();

	TArray<FPrimaryAssetId> AssetIDs;
	AssetManager.GetPrimaryAssetIdList(FPrimaryAssetType("BuildingDefinition"), AssetIDs);

	if (AssetIDs.IsEmpty())
	{
		UE_LOG(LogTemp, Warning,
			TEXT("UBuildingRegistry: No BuildingDefinition assets found. "
			     "Check DefaultGame.ini PrimaryAssetTypesToScan and that assets exist in /Game/Data/Buildings."));
		return;
	}

	AssetManager.LoadPrimaryAssets(
		AssetIDs,
		TArray<FName>(),
		FStreamableDelegate::CreateUObject(this, &UBuildingRegistry::OnDefinitionsLoadCompleted)
	);
}

void UBuildingRegistry::OnDefinitionsLoadCompleted()
{
	UAssetManager& AssetManager = UAssetManager::Get();

	TArray<FPrimaryAssetId> AssetIDs;
	AssetManager.GetPrimaryAssetIdList(FPrimaryAssetType("BuildingDefinition"), AssetIDs);

	for (const FPrimaryAssetId& ID : AssetIDs)
	{
		UBuildingDefinition* Def = Cast<UBuildingDefinition>(AssetManager.GetPrimaryAssetObject(ID));
		if (!Def)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("UBuildingRegistry: Failed to cast asset %s to UBuildingDefinition."),
				*ID.ToString());
			continue;
		}
		Definitions.Add(Def);
	}

	bDefinitionsLoaded = true;
	OnDefinitionsLoaded.Broadcast();

	UE_LOG(LogTemp, Log,
		TEXT("UBuildingRegistry: Loaded %d building definitions."), Definitions.Num());
}

TArray<UBuildingDefinition*> UBuildingRegistry::GetAllDefinitions() const
{
	TArray<UBuildingDefinition*> Result;
	Result.Reserve(Definitions.Num());
	for (const TObjectPtr<UBuildingDefinition>& Def : Definitions)
	{
		if (Def) Result.Add(Def.Get());
	}
	return Result;
}
