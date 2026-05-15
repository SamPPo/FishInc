// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FishIncPlayerController.generated.h"

class UBuildingDefinition;
class ABuildingPreview;
class ABuilding_Base;

UCLASS()
class FISHINC_API AFishIncPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AFishIncPlayerController();

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnCursorMoved, FVector /*WorldPosition*/)
	FOnCursorMoved OnCursorMoved;

	// Broadcast when a building is clicked (passes the building) or empty space is clicked (passes nullptr).
	// FishIncHUD binds to this to show/hide the building upgrade panel.
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnBuildingSelected, ABuilding_Base* /*Building*/)
	FOnBuildingSelected OnBuildingSelected;

	UFUNCTION(BlueprintCallable, Category = "Building",
		meta = (Tooltip = "Enters placement mode for the given building. Left-click confirms placement, right-click cancels."))
	void EnterPlacementMode(UBuildingDefinition* Definition);

	UFUNCTION(BlueprintCallable, Category = "Building",
		meta = (Tooltip = "Exits placement mode without placing a building."))
	void ExitPlacementMode();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Building",
		meta = (Tooltip = "Returns true if the player is currently in building placement mode."))
	bool IsInPlacementMode() const { return bInPlacementMode; }

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;

private:
	FVector LastCursorWorldPosition = FVector::ZeroVector;

	bool bInPlacementMode = false;

	UPROPERTY()
	TObjectPtr<UBuildingDefinition> PendingBuildingDef;

	UPROPERTY()
	TObjectPtr<ABuildingPreview> PreviewActor;

	bool GetCursorFloorPosition(FVector& OutPosition) const;
	bool GetCursorInteractable(FHitResult& OutHit) const;

	void OnInteractPressed();
	void OnCancelPressed();
	void ConfirmPlacement();
};
