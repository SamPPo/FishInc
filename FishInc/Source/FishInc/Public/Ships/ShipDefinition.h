// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ShipDefinition.generated.h"

UCLASS()
class FISHINC_API UShipDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speed",
		meta = (Tooltip = "Base maximum horizontal speed in cm/s, before any upgrades."))
	float BaseSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speed",
		meta = (Tooltip = "UpgradeRegistry stat tag whose resolved value is added to BaseSpeed."))
	FGameplayTag SpeedStatTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fishing",
		meta = (Tooltip = "Base radius in cm within which fish schools are detected and caught."))
	float BaseFishingRadius = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fishing",
		meta = (Tooltip = "UpgradeRegistry stat tag whose resolved value is added to BaseFishingRadius."))
	FGameplayTag FishingRadiusStatTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fishing",
		meta = (Tooltip = "Base time in seconds between successive fish catches."))
	float BaseFishingCooldown = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fishing",
		meta = (Tooltip = "UpgradeRegistry stat tag whose resolved value is added to BaseFishingCooldown. Use negative upgrade values to reduce cooldown."))
	FGameplayTag FishingCooldownStatTag;
};
