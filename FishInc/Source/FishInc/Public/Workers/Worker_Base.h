// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Worker_Base.generated.h"

class ABuilding_Base;

UCLASS(Abstract)
class FISHINC_API AWorker_Base : public AActor
{
	GENERATED_BODY()

public:
	AWorker_Base();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Worker",
		meta = (ToolTip = "Mesh representing this worker in the world."))
	TObjectPtr<UStaticMeshComponent> WorkerMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Worker",
		meta = (ToolTip = "Base interval in seconds between automatic activations of the assigned building."))
	float AutoActivateInterval = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Worker",
		meta = (ToolTip = "Upgrade stat tag whose resolved value modifies AutoActivateInterval. Use negative upgrade values to reduce the interval."))
	FGameplayTag AutoActivateIntervalStatTag;

public:
	// Assigns this worker to a building, removing it from any previous assignment.
	// Fails silently if the building has no available worker slots.
	UFUNCTION(BlueprintCallable, Category = "Worker",
		meta = (ToolTip = "Assigns this worker to a building. Removes it from its current building first. Does nothing if the target building is full."))
	void AssignToBuilding(ABuilding_Base* NewBuilding);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Worker",
		meta = (ToolTip = "Returns the building this worker is currently assigned to, or null if unassigned."))
	ABuilding_Base* GetAssignedBuilding() const { return AssignedBuilding.Get(); }

private:
	UPROPERTY()
	TObjectPtr<ABuilding_Base> AssignedBuilding;

	FTimerHandle AutoActivateTimerHandle;

	void AutoActivate();
	void RefreshAutoActivateInterval();
	void HandleUpgradeStatChanged(FGameplayTag StatTag);
};
