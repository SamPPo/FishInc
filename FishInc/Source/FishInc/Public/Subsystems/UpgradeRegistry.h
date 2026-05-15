// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "UpgradeRegistry.generated.h"

// How a stat contribution stacks. Defined here so BuildingDefinition can reference it.
UENUM(BlueprintType)
enum class EUpgradeType : uint8
{
	Additive,       // Flat value summed across all contributions
	Multiplicative  // Stacking multiplier: each contribution adds to a base of 1.0
};

// Internal record of one building instance's contribution to a single stat.
// Not a USTRUCT — never exposed to Blueprint or UE reflection.
struct FStatContribution
{
	FGameplayTag  StatTag;
	EUpgradeType  UpgradeType = EUpgradeType::Additive;
	float         Value       = 0.f;
};

UCLASS()
class FISHINC_API UUpgradeRegistry : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Called by Building_Base::InitFromDefinition and Building_Base::PurchaseUpgrade.
	// Replaces any previous contributions registered under InstanceID and broadcasts
	// OnStatChanged for every affected tag.
	void RegisterContributions(FName InstanceID, TArray<FStatContribution> Contributions);

	// Called by Building_Base::EndPlay to remove this building's contributions entirely.
	// Broadcasts OnStatChanged for each tag that was being contributed to.
	void UnregisterContributions(FName InstanceID);

	// Sums all registered contributions for StatTag.
	// Additive values are summed first; the total is then scaled by stacked multipliers.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Upgrades",
		meta = (Tooltip = "Returns the combined stat value for the given tag across all placed buildings."))
	float ResolveStat(FGameplayTag StatTag) const;

	// Fired once per affected stat tag after any RegisterContributions or UnregisterContributions call.
	// Ships, workers, and processors bind to this to refresh their cached stat values.
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnStatChanged, FGameplayTag /*AffectedStat*/)
	FOnStatChanged OnStatChanged;

private:
	// Key: unique instance ID assigned by Building_Base on placement.
	// Value: that building's current stat contributions at its current level.
	TMap<FName, TArray<FStatContribution>> RegisteredContributions;
};
