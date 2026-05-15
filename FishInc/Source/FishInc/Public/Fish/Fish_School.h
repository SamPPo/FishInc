// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Fish_School.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FISHINC_API UFish_School : public UInstancedStaticMeshComponent
{
	GENERATED_BODY()

public:
	UFish_School();

protected:
	virtual void BeginPlay() override;

public:
	// Returns evenly spaced locations in a grid across a plane centered on the local origin.
	// CountX/CountY control the number of points along each axis.
	// PlaneX/PlaneY set the total size of the plane along each axis.
	UFUNCTION(BlueprintCallable, Category = "Fish School")
	TArray<FVector> GetEvenlyPlacedLocations(
		UPARAM(meta = (Tooltip = "Number of locations along the X axis")) int32 CountX,
		UPARAM(meta = (Tooltip = "Number of locations along the Y axis")) int32 CountY,
		UPARAM(meta = (Tooltip = "Total size of the plane along the X axis")) float PlaneX,
		UPARAM(meta = (Tooltip = "Total size of the plane along the Y axis")) float PlaneY) const;

	// Removes the given instance indices. Duplicates are filtered and indices
	// are processed highest-first so each removal doesn't shift unprocessed ones.
	UFUNCTION(BlueprintCallable, Category = "Fish School")
	void RemoveFishInstances(const TArray<int32>& InstanceIndices);
};
