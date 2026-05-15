// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buildings/Building_Base.h"
#include "Subsystems/EconomySubsystem.h"
#include "GameplayTagContainer.h"
#include "Building_Processor.generated.h"

UCLASS()
class FISHINC_API ABuilding_Processor : public ABuilding_Base
{
	GENERATED_BODY()

public:
	ABuilding_Processor();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Processor",
		meta = (ToolTip = "Returns true if this processor is currently waiting out its cooldown."))
	bool IsOnCooldown() const { return bOnCooldown; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Processor",
		meta = (ToolTip = "Currency type consumed from EconomySubsystem each processing cycle."))
	ECurrencyType InputCurrencyType = ECurrencyType::Fish;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Processor",
		meta = (ToolTip = "Amount of input currency consumed per processing cycle."))
	double InputAmount = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Processor",
		meta = (ToolTip = "Currency type produced each processing cycle."))
	ECurrencyType OutputCurrencyType = ECurrencyType::Coins;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Processor",
		meta = (ToolTip = "Base amount of output currency produced per cycle before multipliers."))
	double OutputAmount = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Processor",
		meta = (ToolTip = "Base multiplier applied to output. Stacks additively with the resolved upgrade stat."))
	float OutputMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Processor",
		meta = (ToolTip = "Upgrade stat tag whose resolved value is added to OutputMultiplier. Leave unset to use base multiplier only."))
	FGameplayTag OutputMultiplierStatTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Processor",
		meta = (ToolTip = "Minimum seconds between processing cycles. Prevents re-activation until expired."))
	float ProcessCooldown = 1.0f;

	virtual void OnActivated() override;

private:
	bool bOnCooldown = false;
	FTimerHandle CooldownTimerHandle;

	void OnCooldownExpired();
};
