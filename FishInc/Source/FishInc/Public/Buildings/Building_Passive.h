// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buildings/Building_Base.h"
#include "Building_Passive.generated.h"

// A building that contributes passively to stats just by being placed.
// Stat effects are defined in its UBuildingDefinition::Upgrades.
// OnActivated does nothing — passive buildings are not clickable for effect.
UCLASS()
class FISHINC_API ABuilding_Passive : public ABuilding_Base
{
	GENERATED_BODY()

public:
	ABuilding_Passive();

protected:
	virtual void OnActivated() override;
};
