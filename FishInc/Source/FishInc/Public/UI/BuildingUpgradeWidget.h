// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Buildings/Building_Base.h"
#include "BuildingUpgradeWidget.generated.h"

UCLASS()
class FISHINC_API UBuildingUpgradeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Called by FishIncHUD when a building is selected. Pass nullptr to clear the panel.
	UFUNCTION(BlueprintCallable, Category = "UI",
		meta = (Tooltip = "Sets the building this panel represents and notifies Blueprint to refresh the layout."))
	void SetBuilding(ABuilding_Base* InBuilding);

	// Called from the Blueprint upgrade button. Spends coins and advances the building one level.
	// Returns true if the upgrade succeeded.
	UFUNCTION(BlueprintCallable, Category = "UI",
		meta = (Tooltip = "Attempts to purchase the next upgrade level for the selected building. Returns true on success."))
	bool ExecuteUpgrade();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI",
		meta = (Tooltip = "Display name from the building's definition. Empty if no building is set."))
	FText GetBuildingDisplayName() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI",
		meta = (Tooltip = "Current upgrade level of the selected building."))
	int32 GetCurrentLevel() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI",
		meta = (Tooltip = "Maximum upgrade level defined in the building's definition."))
	int32 GetMaxLevel() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI",
		meta = (Tooltip = "Formatted coin cost for the next upgrade level, or MAX if the building is fully upgraded."))
	FText GetUpgradeCostText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI",
		meta = (Tooltip = "Returns true if the player can afford the upgrade and the building is not at max level."))
	bool CanUpgradeBuilding() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI",
		meta = (Tooltip = "Returns the building this widget is representing. Use this in Blueprint to call NotifyWidgetHoverBegin/End on it."))
	ABuilding_Base* GetCachedBuilding() const { return CachedBuilding.Get(); }

protected:
	// Override in Blueprint to refresh all display elements after SetBuilding is called.
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnBuildingSet();

	// Override in Blueprint to react after a successful upgrade (update text, play animation, etc.).
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnUpgradePurchased();



private:
	// Nulled automatically by UE if the building actor is destroyed while the panel is open.
	UPROPERTY()
	TObjectPtr<ABuilding_Base> CachedBuilding;
};
