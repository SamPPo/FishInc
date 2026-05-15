// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Subsystems/LoopStateSubsystem.h"
#include "PlayerPawn_Base.generated.h"

UCLASS()
class FISHINC_API APlayerPawn_Base : public APawn
{
	GENERATED_BODY()

public:
	APlayerPawn_Base();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Implement in Blueprint to react to loop transitions — camera cuts, UI swaps, etc.
	UFUNCTION(BlueprintImplementableEvent, Category = "Fishing",
		meta = (Tooltip = "Called when the game loop switches between Fishing and Village. Implement in Blueprint for camera or visual transitions."))
	void OnLoopStateChanged(ELoopState OldState, ELoopState NewState);

private:
	void HandleLoopStateChanged(ELoopState OldState, ELoopState NewState);
};
