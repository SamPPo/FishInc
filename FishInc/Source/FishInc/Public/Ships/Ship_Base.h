// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Subsystems/LoopStateSubsystem.h"
#include "Ship_Base.generated.h"

class UShipDefinition;

UCLASS(meta = (PrioritizeCategories = "Ship Ship|Movement Ship|Stats"))
class FISHINC_API AShip_Base : public AActor
{
	GENERATED_BODY()

public:
	AShip_Base();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;

	void UpdateCursorTarget(FVector WorldPosition);

	UFUNCTION(BlueprintCallable, Category = "Ship",
		meta = (Tooltip = "Deposits all fish stored in the hold into the EconomySubsystem and clears the hold. Call this when the fishing loop ends."))
	void TransferFishToEconomy();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "0 | Ship",
		meta = (Tooltip = "Physics-simulated root mesh. Assign a mesh and enable physics in the Blueprint."))
	TObjectPtr<UStaticMeshComponent> ShipMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "0 | Ship|Stats",
		meta = (Tooltip = "Fish currently held in the ship's hold. Cleared when TransferFishToEconomy is called."))
	double FishStoredAmount = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0 | Ship|Stats",
		meta = (Tooltip = "Maximum fish this ship can hold. ScanForFish stops adding once the hold is full."))
	double FishStorageCapacity = 100.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "0 | Ship",
		meta = (Tooltip = "Defines base stats for this ship type. Stats are further modified by UpgradeRegistry."))
	TObjectPtr<UShipDefinition> ShipDefinition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0 | Ship|Movement",
		meta = (Tooltip = "Smoothing zone radius in cm."))
	float ArrivalRadius = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "0 | Ship|Stats",
		meta = (Tooltip = "Current radius in cm within which fish schools are detected and caught."))
	float FishingRadius = 300.f;
	
	// Physics tuning — separate from gameplay stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0 | Ship|Stats",
		meta = (Tooltip = "Horizontal force in Newtons applied each tick when steering toward the target."))
	float MovementForce = 2000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "0 | Ship|Stats",
		meta = (Tooltip = "Current time in seconds between successive fish catches."))
	float FishingCooldown = 5.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "0 | Ship|Stats",
		meta = (Tooltip = "Amount of fish caught per tick."))
	float FishingAmountPerTick = 1.f;

private:
	FVector TargetPosition = FVector::ZeroVector;
	float FishingCooldownTimer = 0.f;
	bool bInFishingLoop = true;

	void ApplyMovementForce(float DeltaTime);
	void ScanForFish();

	void RefreshStatsFromRegistry();
	void HandleUpgradeStatChanged(FGameplayTag StatTag);
	void HandleLoopStateChanged(ELoopState OldState, ELoopState NewState);
};
