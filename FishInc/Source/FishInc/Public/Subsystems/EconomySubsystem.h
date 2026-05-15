// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EconomySubsystem.generated.h"

UENUM(BlueprintType)
enum class ECurrencyType : uint8
{
	Fish   UMETA(DisplayName = "Fish"),
	Coins  UMETA(DisplayName = "Coins"),
	Pearls UMETA(DisplayName = "Pearls")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrencyChanged, ECurrencyType, CurrencyType, double, NewAmount);

UCLASS()
class FISHINC_API UEconomySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Economy")
	void AddCurrency(ECurrencyType CurrencyType, double Amount);

	UFUNCTION(BlueprintCallable, Category = "Economy")
	bool Spend(ECurrencyType CurrencyType, double Amount);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Economy")
	double GetCurrencyAmount(ECurrencyType CurrencyType) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Economy")
	bool CanAfford(ECurrencyType CurrencyType, double Amount) const;

	UPROPERTY(BlueprintAssignable, Category = "Economy", meta = (Tooltip = "Broadcast when a currency amount changes, passing the currency type and the new amount"))
	FOnCurrencyChanged OnCurrencyChanged;

private:
	UPROPERTY(meta = (Tooltip = "Current balance for each currency type"))
	TMap<ECurrencyType, double> CurrencyBalances;
};
