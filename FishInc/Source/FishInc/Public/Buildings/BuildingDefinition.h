// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Buildings/Building_Base.h"
#include "Subsystems/UpgradeRegistry.h"
#include "BuildingDefinition.generated.h"

// One stat effect applied to a building at each upgrade level.
// A building definition can have multiple of these — one per stat it affects.
USTRUCT(BlueprintType)
struct FBuildingUpgrade
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Upgrades",
		meta = (Tooltip = "Gameplay tag identifying which stat this upgrade modifies."))
	FGameplayTag AffectedStat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Upgrades",
		meta = (Tooltip = "Additive: flat value summed per level. Multiplicative: stacking percentage where 0.1 = +10%."))
	EUpgradeType UpgradeType = EUpgradeType::Additive;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Upgrades",
		meta = (ClampMin = "0.0", Tooltip = "Stat contribution at level 1 (the initial placed state)."))
	float BaseValue = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Upgrades",
		meta = (ClampMin = "0.0", Tooltip = "Additional stat contribution added for each level beyond level 1."))
	float ValuePerLevel = 0.f;

	float GetValueAtLevel(int32 Level) const
	{
		if (Level <= 0) return 0.f;
		return BaseValue + ValuePerLevel * (Level - 1);
	}
};

UCLASS(BlueprintType)
class FISHINC_API UBuildingDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(FPrimaryAssetType("BuildingDefinition"), GetFName());
	}

	// --- Display ---

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building",
		meta = (Tooltip = "Name shown in the building shop."))
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building",
		meta = (Tooltip = "Description shown in the building shop."))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building",
		meta = (Tooltip = "Thumbnail shown in the building shop."))
	TObjectPtr<UTexture2D> Icon;

	// --- Placement ---

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Economy",
		meta = (ClampMin = "0.0", Tooltip = "Coin cost to buy and place this building."))
	double PurchaseCost = 100.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building",
		meta = (Tooltip = "Actor class spawned when the player confirms placement."))
	TSubclassOf<ABuilding_Base> BuildingClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building",
		meta = (Tooltip = "Mesh shown on the placement preview actor while the player is choosing a location."))
	TObjectPtr<UStaticMesh> PreviewMesh;

	// --- Upgrades ---

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Upgrades",
		meta = (ClampMin = "1", Tooltip = "Maximum level this building can reach. Level 1 is the initial placed state."))
	int32 MaxUpgradeLevel = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Upgrades",
		meta = (ClampMin = "0.0", Tooltip = "Coin cost to upgrade from level 1 to level 2. Subsequent levels scale by UpgradeCostGrowthFactor."))
	double UpgradeCostBase = 100.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Upgrades",
		meta = (ClampMin = "1.0", Tooltip = "Exponential cost multiplier per upgrade. 1.5 means each level costs 50% more than the previous."))
	float UpgradeCostGrowthFactor = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Upgrades",
		meta = (Tooltip = "Stat effects applied when this building is placed and upgraded. Each entry defines one stat that scales with building level."))
	TArray<FBuildingUpgrade> Upgrades;

	// Returns the coin cost to upgrade from CurrentLevel to CurrentLevel+1.
	// CurrentLevel 1 returns UpgradeCostBase; each subsequent level multiplies by UpgradeCostGrowthFactor.
	double GetUpgradeCostAtLevel(int32 CurrentLevel) const
	{
		return UpgradeCostBase * FMath::Pow(UpgradeCostGrowthFactor, static_cast<float>(CurrentLevel - 1));
	}
};
