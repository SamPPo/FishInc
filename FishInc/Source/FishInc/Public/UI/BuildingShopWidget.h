// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildingShopWidget.generated.h"

class UBuildingDefinition;
class UBuildingRegistry;

UCLASS()
class FISHINC_API UBuildingShopWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Called from a shop entry widget's button to begin the buy-and-place flow.
	// Deducts cost and enters placement mode; the player then clicks to place.
	UFUNCTION(BlueprintCallable, Category = "Shop",
		meta = (Tooltip = "Initiates purchase and placement of the given building definition."))
	void BuyBuilding(UBuildingDefinition* Definition);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Shop",
		meta = (Tooltip = "Returns all building definitions loaded from the registry. Safe to call only after OnShopBuilt fires."))
	TArray<UBuildingDefinition*> GetAvailableBuildings() const;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// Called once building definitions are ready. Override in Blueprint to populate the shop entry list.
	UFUNCTION(BlueprintImplementableEvent, Category = "Shop")
	void OnShopBuilt();

private:
	void HandleDefinitionsLoaded();
};
