// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "Subsystems/EconomySubsystem.h"
#include "FishIncCheatManager.generated.h"

UCLASS()
class FISHINC_API UFishIncCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	// Console: GiveCurrency Coins 1000  |  GiveCurrency Fish 500  |  GiveCurrency Pearls 10
	UFUNCTION(Exec)
	void GiveCurrency(ECurrencyType CurrencyType, double Amount);
};
