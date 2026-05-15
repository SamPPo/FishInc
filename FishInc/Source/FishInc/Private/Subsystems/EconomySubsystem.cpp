// Fill out your copyright notice in the Description page of Project Settings.

#include "FishInc/Public/Subsystems/EconomySubsystem.h"

void UEconomySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CurrencyBalances.Add(ECurrencyType::Fish,   0.0);
	CurrencyBalances.Add(ECurrencyType::Coins,  0.0);
	CurrencyBalances.Add(ECurrencyType::Pearls, 0.0);
}

void UEconomySubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UEconomySubsystem::AddCurrency(ECurrencyType CurrencyType, double Amount)
{
	if (Amount <= 0.0)
	{
		return;
	}

	CurrencyBalances[CurrencyType] += Amount;
	OnCurrencyChanged.Broadcast(CurrencyType, CurrencyBalances[CurrencyType]);
}

bool UEconomySubsystem::Spend(ECurrencyType CurrencyType, double Amount)
{
	if (!CanAfford(CurrencyType, Amount))
	{
		return false;
	}

	CurrencyBalances[CurrencyType] -= Amount;
	OnCurrencyChanged.Broadcast(CurrencyType, CurrencyBalances[CurrencyType]);
	return true;
}

double UEconomySubsystem::GetCurrencyAmount(ECurrencyType CurrencyType) const
{
	const double* Balance = CurrencyBalances.Find(CurrencyType);
	return Balance ? *Balance : 0.0;
}

bool UEconomySubsystem::CanAfford(ECurrencyType CurrencyType, double Amount) const
{
	return GetCurrencyAmount(CurrencyType) >= Amount;
}
