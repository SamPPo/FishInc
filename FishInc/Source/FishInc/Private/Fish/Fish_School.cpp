// Fill out your copyright notice in the Description page of Project Settings.

#include "FishInc/Public/Fish/Fish_School.h"

UFish_School::UFish_School()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFish_School::BeginPlay()
{
	Super::BeginPlay();
}

TArray<FVector> UFish_School::GetEvenlyPlacedLocations(int32 CountX, int32 CountY, float PlaneX, float PlaneY) const
{
	TArray<FVector> Locations;

	if (CountX <= 0 || CountY <= 0)
	{
		return Locations;
	}

	Locations.Reserve(CountX * CountY);

	const float StepX = (CountX > 1) ? PlaneX / (CountX - 1) : 0.f;
	const float StepY = (CountY > 1) ? PlaneY / (CountY - 1) : 0.f;
	const float StartX = -PlaneX * 0.5f;
	const float StartY = -PlaneY * 0.5f;

	for (int32 Xi = 0; Xi < CountX; ++Xi)
	{
		for (int32 Yi = 0; Yi < CountY; ++Yi)
		{
			const float X = (CountX > 1) ? StartX + Xi * StepX : 0.f;
			const float Y = (CountY > 1) ? StartY + Yi * StepY : 0.f;
			Locations.Add(FVector(X, Y, 0.f));
		}
	}

	return Locations;
}

void UFish_School::RemoveFishInstances(const TArray<int32>& InstanceIndices)
{
	if (InstanceIndices.IsEmpty()) return;

	// Deduplicate via TSet, then sort descending so removing a high index
	// never invalidates the lower indices still waiting to be removed.
	TArray<int32> Sorted = TSet<int32>(InstanceIndices).Array();
	Sorted.Sort([](int32 A, int32 B) { return A > B; });

	for (int32 Index : Sorted)
	{
		if (Index >= 0 && Index < GetInstanceCount())
		{
			RemoveInstance(Index);
		}
	}
}
