// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/FishIncCheatManager.h"

void UFishIncCheatManager::GiveCurrency(ECurrencyType CurrencyType, double Amount)
{
	APlayerController* PC = GetOuterAPlayerController();
	ensureMsgf(PC, TEXT("UFishIncCheatManager::GiveCurrency — PlayerController is null"));
	if (!PC) return;

	UGameInstance* GI = PC->GetGameInstance();
	ensureMsgf(GI, TEXT("UFishIncCheatManager::GiveCurrency — GameInstance is null"));
	if (!GI) return;

	UEconomySubsystem* Economy = GI->GetSubsystem<UEconomySubsystem>();
	ensureMsgf(Economy, TEXT("UFishIncCheatManager::GiveCurrency — EconomySubsystem not found"));
	if (!Economy) return;

	Economy->AddCurrency(CurrencyType, Amount);
}
