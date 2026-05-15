// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/UpgradeRegistry.h"
#include "Subsystems/EconomySubsystem.h"

void UUpgradeRegistry::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Collection.InitializeDependency<UEconomySubsystem>();
}

void UUpgradeRegistry::Deinitialize()
{
	RegisteredContributions.Empty();
	Super::Deinitialize();
}

void UUpgradeRegistry::RegisterContributions(FName InstanceID, TArray<FStatContribution> Contributions)
{
	// Collect all stat tags that will be affected (old and new) for broadcasting.
	TSet<FGameplayTag> AffectedTags;

	if (const TArray<FStatContribution>* Existing = RegisteredContributions.Find(InstanceID))
	{
		for (const FStatContribution& C : *Existing)
			AffectedTags.Add(C.StatTag);
	}
	for (const FStatContribution& C : Contributions)
		AffectedTags.Add(C.StatTag);

	if (Contributions.IsEmpty())
		RegisteredContributions.Remove(InstanceID);
	else
		RegisteredContributions.FindOrAdd(InstanceID) = MoveTemp(Contributions);

	for (const FGameplayTag& Tag : AffectedTags)
		OnStatChanged.Broadcast(Tag);
}

void UUpgradeRegistry::UnregisterContributions(FName InstanceID)
{
	const TArray<FStatContribution>* Existing = RegisteredContributions.Find(InstanceID);
	if (!Existing) return;

	TSet<FGameplayTag> AffectedTags;
	for (const FStatContribution& C : *Existing)
		AffectedTags.Add(C.StatTag);

	RegisteredContributions.Remove(InstanceID);

	for (const FGameplayTag& Tag : AffectedTags)
		OnStatChanged.Broadcast(Tag);
}

float UUpgradeRegistry::ResolveStat(FGameplayTag StatTag) const
{
	float Additive       = 0.f;
	float Multiplicative = 1.f;

	for (const auto& [ID, Contributions] : RegisteredContributions)
	{
		for (const FStatContribution& C : Contributions)
		{
			if (!C.StatTag.MatchesTagExact(StatTag)) continue;

			if (C.UpgradeType == EUpgradeType::Additive)
				Additive += C.Value;
			else
				Multiplicative += C.Value;
		}
	}

	return Additive * Multiplicative;
}
