// Fill out your copyright notice in the Description page of Project Settings.

#include "Buildings/Building_Special.h"

ABuilding_Special::ABuilding_Special()
{
	Category = EBuildingCategory::Special;
}

void ABuilding_Special::OnActivated()
{
	Super::OnActivated();
}
