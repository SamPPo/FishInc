// Fill out your copyright notice in the Description page of Project Settings.

#include "Buildings/Building_Passive.h"

ABuilding_Passive::ABuilding_Passive()
{
	Category = EBuildingCategory::Passive;
}

void ABuilding_Passive::OnActivated()
{
	// Passive buildings have no click effect — intentionally empty.
}
