// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LoopStateSubsystem.generated.h"

UENUM(BlueprintType)
enum class ELoopState : uint8
{
	Fishing UMETA(DisplayName = "Fishing"),
	Village UMETA(DisplayName = "Village")
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLoopStateChanged, ELoopState /*OldState*/, ELoopState /*NewState*/);

UCLASS()
class FISHINC_API ULoopStateSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Fishing",
		meta = (Tooltip = "Switches the active game loop and notifies all subscribers. No-op if NewState matches the current state."))
	void SetLoopState(ELoopState NewState);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Fishing",
		meta = (Tooltip = "Returns the currently active game loop state."))
	ELoopState GetLoopState() const { return CurrentState; }

	FOnLoopStateChanged OnLoopStateChanged;

private:
	ELoopState CurrentState = ELoopState::Fishing;
};
