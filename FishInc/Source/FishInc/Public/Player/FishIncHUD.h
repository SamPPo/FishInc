// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Subsystems/EconomySubsystem.h"
#include "UI/LoopToggleWidget.h"
#include "UI/BuildingShopWidget.h"
#include "UI/BuildingUpgradeWidget.h"
#include "FishIncHUD.generated.h"

class ABuilding_Base;

UCLASS()
class FISHINC_API AFishIncHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void DrawHUD() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI",
		meta = (Tooltip = "Font used to draw the resource counter. Falls back to the engine default if unset."))
	TObjectPtr<UFont> HUDFont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI",
		meta = (Tooltip = "Scale of the resource counter text."))
	float FontScale = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI",
		meta = (Tooltip = "Padding from the top-left corner of the viewport in pixels."))
	FVector2D CounterPadding = FVector2D(20.f, 20.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI",
		meta = (Tooltip = "Blueprint widget subclass to use for the loop toggle button. Assign a Blueprint that inherits from ULoopToggleWidget."))
	TSubclassOf<ULoopToggleWidget> LoopToggleWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI",
		meta = (Tooltip = "Blueprint widget subclass to use for the building shop. Assign a Blueprint that inherits from UBuildingShopWidget."))
	TSubclassOf<UBuildingShopWidget> BuildingShopWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI",
		meta = (Tooltip = "Blueprint widget subclass to use for the building upgrade panel. Assign a Blueprint that inherits from UBuildingUpgradeWidget."))
	TSubclassOf<UBuildingUpgradeWidget> BuildingUpgradeWidgetClass;

private:
	UPROPERTY()
	TObjectPtr<ULoopToggleWidget> LoopToggleWidgetInstance;

	UPROPERTY()
	TObjectPtr<UBuildingShopWidget> BuildingShopWidgetInstance;

	UPROPERTY()
	TObjectPtr<UBuildingUpgradeWidget> BuildingUpgradeWidgetInstance;

	UFUNCTION()
	void HandleCurrencyChanged(ECurrencyType CurrencyType, double NewAmount);

	void HandleBuildingSelected(ABuilding_Base* Building);

	double CachedFish   = 0.0;
	double CachedCoins  = 0.0;
	double CachedPearls = 0.0;
};
