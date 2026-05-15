// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/BuildingUpgradeWidget.h"
#include "Buildings/Building_Base.h"
#include "Buildings/BuildingDefinition.h"

void UBuildingUpgradeWidget::SetBuilding(ABuilding_Base* InBuilding)
{
	CachedBuilding = InBuilding;
	OnBuildingSet();
}

bool UBuildingUpgradeWidget::ExecuteUpgrade()
{
	if (!CachedBuilding) return false;
	if (!CachedBuilding->PurchaseUpgrade()) return false;

	OnUpgradePurchased();
	return true;
}

FText UBuildingUpgradeWidget::GetBuildingDisplayName() const
{
	if (!CachedBuilding) return FText::GetEmpty();

	const UBuildingDefinition* Def = CachedBuilding->GetDefinition();
	return Def ? Def->DisplayName : FText::GetEmpty();
}

int32 UBuildingUpgradeWidget::GetCurrentLevel() const
{
	return CachedBuilding ? CachedBuilding->GetBuildingLevel() : 0;
}

int32 UBuildingUpgradeWidget::GetMaxLevel() const
{
	if (!CachedBuilding) return 0;

	const UBuildingDefinition* Def = CachedBuilding->GetDefinition();
	return Def ? Def->MaxUpgradeLevel : 0;
}

FText UBuildingUpgradeWidget::GetUpgradeCostText() const
{
	if (!CachedBuilding || CachedBuilding->IsAtMaxLevel())
		return FText::FromString(TEXT("MAX"));

	return FText::FromString(FString::Printf(TEXT("%.0f"), CachedBuilding->GetUpgradeCost()));
}

bool UBuildingUpgradeWidget::CanUpgradeBuilding() const
{
	return CachedBuilding && CachedBuilding->CanUpgrade();
}


