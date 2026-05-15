// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buildings/Building_Base.h"
#include "Building_Dock.generated.h"

UCLASS()
class FISHINC_API ABuilding_Dock : public ABuilding_Base
{
	GENERATED_BODY()

public:
	ABuilding_Dock();

protected:
	// Calls TransferFishToEconomy on every ship in the world.
	virtual void OnActivated() override;
};
