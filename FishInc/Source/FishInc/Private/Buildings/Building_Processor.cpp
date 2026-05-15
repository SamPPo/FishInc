// Fill out your copyright notice in the Description page of Project Settings.

#include "Buildings/Building_Processor.h"
#include "Subsystems/EconomySubsystem.h"
#include "Subsystems/UpgradeRegistry.h"

ABuilding_Processor::ABuilding_Processor()
{
	Category = EBuildingCategory::Processor;
}

void ABuilding_Processor::OnActivated()
{
	if (bOnCooldown) return;

	UGameInstance* GI = GetGameInstance();
	ensureMsgf(GI, TEXT("ABuilding_Processor::OnActivated — GameInstance is null"));
	if (!GI) return;

	UEconomySubsystem* Economy = GI->GetSubsystem<UEconomySubsystem>();
	ensureMsgf(Economy, TEXT("ABuilding_Processor::OnActivated — UEconomySubsystem not found"));
	if (!Economy) return;

	if (!Economy->Spend(InputCurrencyType, InputAmount)) return;

	float ResolvedMultiplier = OutputMultiplier;
	if (OutputMultiplierStatTag.IsValid())
	{
		UUpgradeRegistry* Registry = GI->GetSubsystem<UUpgradeRegistry>();
		ensureMsgf(Registry, TEXT("ABuilding_Processor::OnActivated — UUpgradeRegistry not found"));
		if (Registry)
		{
			ResolvedMultiplier += Registry->ResolveStat(OutputMultiplierStatTag);
		}
	}

	Economy->AddCurrency(OutputCurrencyType, OutputAmount * static_cast<double>(ResolvedMultiplier));

	bOnCooldown = true;
	GetWorldTimerManager().SetTimer(CooldownTimerHandle, this, &ABuilding_Processor::OnCooldownExpired, ProcessCooldown, false);

	Super::OnActivated();
}

void ABuilding_Processor::OnCooldownExpired()
{
	bOnCooldown = false;
}
