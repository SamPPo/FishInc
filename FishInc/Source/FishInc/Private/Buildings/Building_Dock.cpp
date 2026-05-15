// Fill out your copyright notice in the Description page of Project Settings.

#include "Buildings/Building_Dock.h"
#include "Ships/Ship_Base.h"
#include "EngineUtils.h"

ABuilding_Dock::ABuilding_Dock()
{
	Category = EBuildingCategory::Storage;
}

void ABuilding_Dock::OnActivated()
{
	for (TActorIterator<AShip_Base> It(GetWorld()); It; ++It)
	{
		It->TransferFishToEconomy();
	}

	Super::OnActivated();
}
