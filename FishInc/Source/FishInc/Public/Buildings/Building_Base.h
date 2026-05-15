// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "Building_Base.generated.h"

class AWorker_Base;
class UBuildingDefinition;
class UWidgetComponent;
class UBuildingUpgradeWidget;

UENUM(BlueprintType)
enum class EBuildingCategory : uint8
{
	Storage		UMETA(DisplayName = "Storage"),
	Processor	UMETA(DisplayName = "Processor"),
	Passive		UMETA(DisplayName = "Passive"),
	Special		UMETA(DisplayName = "Special")
};

UCLASS(Abstract)
class FISHINC_API ABuilding_Base : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ABuilding_Base();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building",
		meta = (ToolTip = "Root mesh representing this building in the world."))
	TObjectPtr<UStaticMeshComponent> BuildingMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI",
		meta = (ToolTip = "Screen-space widget component that displays the upgrade panel when the player hovers this building."))
	TObjectPtr<UWidgetComponent> UpgradeWidgetComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI",
		meta = (ToolTip = "Widget Blueprint class to use for the upgrade panel. Must inherit from UBuildingUpgradeWidget."))
	TSubclassOf<UBuildingUpgradeWidget> UpgradeWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building",
		meta = (ToolTip = "Functional category of this building."))
	EBuildingCategory Category;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building",
		meta = (ToolTip = "Maximum number of workers that can be assigned to this building at once."))
	int32 MaxWorkers = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building",
		meta = (ToolTip = "Definition this building was spawned from. Set by InitFromDefinition before BeginPlay."))
	TObjectPtr<UBuildingDefinition> Definition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building",
		meta = (ToolTip = "Current upgrade level. Starts at 1 on placement; increases each time PurchaseUpgrade succeeds."))
	int32 BuildingLevel = 0;

public:
	// IInteractable — routes to OnActivated
	virtual void Interact_Implementation() override;

	// Called by FishIncPlayerController during deferred spawn, before BeginPlay.
	// Sets the definition and registers level-1 stat contributions with UpgradeRegistry.
	UFUNCTION(BlueprintCallable, Category = "Building",
		meta = (Tooltip = "Initialises this building from its definition. Must be called before FinishSpawning in a deferred spawn."))
	void InitFromDefinition(UBuildingDefinition* InDefinition);

	// Spends coins and advances this building to the next upgrade level.
	// Returns true if the purchase succeeded.
	UFUNCTION(BlueprintCallable, Category = "Building",
		meta = (Tooltip = "Purchases the next upgrade level for this building. Deducts coins and updates registered stat contributions."))
	bool PurchaseUpgrade();

	// Coin cost to go from the current level to the next.
	// Returns 0 if at max level or definition is not set.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Building",
		meta = (Tooltip = "Returns the coin cost of the next upgrade level for this building."))
	double GetUpgradeCost() const;

	// Returns true if the player can afford the next upgrade and this building is not at max level.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Building",
		meta = (Tooltip = "Returns true if the player can afford to upgrade this building and it is not at max level."))
	bool CanUpgrade() const;

	// Returns true if BuildingLevel has reached MaxUpgradeLevel defined in the definition.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Building",
		meta = (Tooltip = "Returns true if this building has reached its maximum upgrade level."))
	bool IsAtMaxLevel() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Building",
		meta = (Tooltip = "Returns the current upgrade level of this building."))
	int32 GetBuildingLevel() const { return BuildingLevel; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Building",
		meta = (Tooltip = "Returns the definition this building was spawned from, or null if InitFromDefinition has not been called."))
	UBuildingDefinition* GetDefinition() const { return Definition.Get(); }

	bool AssignWorker(AWorker_Base* Worker);
	bool RemoveWorker(AWorker_Base* Worker);
	bool HasWorkerSlotAvailable() const;
	const TArray<TObjectPtr<AWorker_Base>>& GetWorkers() const { return Workers; }

	UFUNCTION(BlueprintCallable, Category = "UI",
		meta = (Tooltip = "Call from the upgrade widget when the mouse enters it to cancel the pending hide timer."))
	void NotifyWidgetHoverBegin();

	UFUNCTION(BlueprintCallable, Category = "UI",
		meta = (Tooltip = "Call from the upgrade widget when the mouse leaves it to restart the hide timer."))
	void NotifyWidgetHoverEnd();


	DECLARE_MULTICAST_DELEGATE_OneParam(FOnBuildingActivated, ABuilding_Base* /*Building*/)
	FOnBuildingActivated OnBuildingActivated;

protected:
	// Override in subclasses to define activation behaviour. Call Super to broadcast OnBuildingActivated.
	virtual void OnActivated();

private:
	// Unique ID generated in InitFromDefinition. Used as the key in UpgradeRegistry.
	FName BuildingInstanceID;

	FTimerHandle HideWidgetTimerHandle;

	void ShowUpgradeWidget();
	void HideUpgradeWidget();

	UFUNCTION()
	void HandleActorBeginCursorOver(AActor* TouchedActor);

	UFUNCTION()
	void HandleActorEndCursorOver(AActor* TouchedActor);

	// Recomputes stat contributions from Definition->Upgrades at BuildingLevel
	// and pushes them to UpgradeRegistry.
	void RefreshRegistryContributions();

	UPROPERTY()
	TArray<TObjectPtr<AWorker_Base>> Workers;
};
