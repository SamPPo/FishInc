// Fill out your copyright notice in the Description page of Project Settings.

#include "Ships/Ship_Base.h"
#include "Ships/ShipDefinition.h"
#include "Fish/Fish_School.h"
#include "Subsystems/UpgradeRegistry.h"
#include "Subsystems/EconomySubsystem.h"
#include "Player/FishIncPlayerController.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/OverlapResult.h"

float ArrivalRadiusSquared = 0.f;

AShip_Base::AShip_Base()
{
	PrimaryActorTick.bCanEverTick = true;
/*
	ShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	SetRootComponent(ShipMesh);
	ShipMesh->SetSimulatePhysics(true);
	ShipMesh->SetLinearDamping(2.f);
	ShipMesh->SetAngularDamping(5.f);
*/
}

void AShip_Base::BeginPlay()
{
	Super::BeginPlay();
	TargetPosition = GetActorLocation();

	UGameInstance* GI = GetGameInstance();
	ensureMsgf(GI, TEXT("AShip_Base::BeginPlay — GameInstance is null"));
	if (GI)
	{
		UUpgradeRegistry* Registry = GI->GetSubsystem<UUpgradeRegistry>();
		ensureMsgf(Registry, TEXT("AShip_Base::BeginPlay — UUpgradeRegistry not found"));
		if (Registry)
		{
			Registry->OnStatChanged.AddUObject(this, &AShip_Base::HandleUpgradeStatChanged);
		}

		ULoopStateSubsystem* LoopState = GI->GetSubsystem<ULoopStateSubsystem>();
		ensureMsgf(LoopState, TEXT("AShip_Base::BeginPlay — ULoopStateSubsystem not found"));
		if (LoopState)
		{
			bInFishingLoop = (LoopState->GetLoopState() == ELoopState::Fishing);
			LoopState->OnLoopStateChanged.AddUObject(this, &AShip_Base::HandleLoopStateChanged);
		}
	}

	if (AFishIncPlayerController* PC = Cast<AFishIncPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		PC->OnCursorMoved.AddUObject(this, &AShip_Base::UpdateCursorTarget);
	}

	// Apply data asset base values and any already-loaded upgrade bonuses
	RefreshStatsFromRegistry();
	ArrivalRadiusSquared = ArrivalRadius * ArrivalRadius;
}

void AShip_Base::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UUpgradeRegistry* Registry = GI->GetSubsystem<UUpgradeRegistry>())
		{
			Registry->OnStatChanged.RemoveAll(this);
		}

		if (ULoopStateSubsystem* LoopState = GI->GetSubsystem<ULoopStateSubsystem>())
		{
			LoopState->OnLoopStateChanged.RemoveAll(this);
		}
	}

	if (AFishIncPlayerController* PC = Cast<AFishIncPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		PC->OnCursorMoved.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void AShip_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ApplyMovementForce(DeltaTime);

	FishingCooldownTimer -= DeltaTime;
	if (FishingCooldownTimer <= 0.f)
	{
		FishingCooldownTimer = FishingCooldown;
		ScanForFish();
	}
}

void AShip_Base::UpdateCursorTarget(FVector WorldPosition)
{
	TargetPosition.X = WorldPosition.X;
	TargetPosition.Y = WorldPosition.Y;
	// Z is kept unchanged — locked by physics constraint
}

void AShip_Base::ApplyMovementForce(float DeltaTime)
{
	const FVector CurrentPos = GetActorLocation();
	FVector ToTarget = TargetPosition - CurrentPos;
	ToTarget.Z = 0.f;
	float DistanceAlpha = ToTarget.SizeSquared2D() / ArrivalRadiusSquared;
	DistanceAlpha = FMath::Clamp(DistanceAlpha, 0.f, 1.f);
	DistanceAlpha *= DistanceAlpha;

	const FVector Direction = ToTarget.GetSafeNormal2D();

	ShipMesh->AddImpulse((Direction * MovementForce * DeltaTime * DistanceAlpha), NAME_None, true);
}

void AShip_Base::ScanForFish()
{
	if (!bInFishingLoop) return;

	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByChannel(
		Overlaps, GetActorLocation(), FQuat::Identity,
		ECC_GameTraceChannel2, FCollisionShape::MakeSphere(FishingRadius));

	const int32 MaxHits = FMath::Max(1, FMath::TruncToInt(FishingAmountPerTick));

	// Group instance indices by their Fish_School component
	TMap<UFish_School*, TArray<int32>> SchoolHits;
	int32 TotalHits = 0;

	for (const FOverlapResult& Overlap : Overlaps)
	{
		if (TotalHits >= MaxHits) break;

		UFish_School* School = Cast<UFish_School>(Overlap.GetComponent());
		if (!School) continue;

		SchoolHits.FindOrAdd(School).Add(Overlap.ItemIndex);
		TotalHits++;
	}

	for (auto& [School, Indices] : SchoolHits)
	{
		School->RemoveFishInstances(Indices);
	}

	if (TotalHits > 0)
	{
		const double Space = FishStorageCapacity - FishStoredAmount;
		FishStoredAmount += FMath::Min(static_cast<double>(TotalHits), Space);
	}
}

void AShip_Base::RefreshStatsFromRegistry()
{
	if (!ShipDefinition) return;

	UGameInstance* GI = GetGameInstance();
	ensureMsgf(GI, TEXT("AShip_Base::RefreshStatsFromRegistry — GameInstance is null"));
	if (!GI) return;

	UUpgradeRegistry* Registry = GI->GetSubsystem<UUpgradeRegistry>();
	ensureMsgf(Registry, TEXT("AShip_Base::RefreshStatsFromRegistry — UUpgradeRegistry not found"));
	if (!Registry) return;

	MovementForce       = ShipDefinition->BaseSpeed         + Registry->ResolveStat(ShipDefinition->SpeedStatTag);
	FishingRadius  = ShipDefinition->BaseFishingRadius  + Registry->ResolveStat(ShipDefinition->FishingRadiusStatTag);
	FishingCooldown = FMath::Max(0.1f,
		ShipDefinition->BaseFishingCooldown + Registry->ResolveStat(ShipDefinition->FishingCooldownStatTag));
}

void AShip_Base::TransferFishToEconomy()
{
	if (FishStoredAmount <= 0.0) return;

	UGameInstance* GI = GetGameInstance();
	ensureMsgf(GI, TEXT("AShip_Base::TransferFishToEconomy — GameInstance is null"));
	if (!GI) return;

	UEconomySubsystem* Economy = GI->GetSubsystem<UEconomySubsystem>();
	ensureMsgf(Economy, TEXT("AShip_Base::TransferFishToEconomy — UEconomySubsystem not found"));
	if (!Economy) return;

	Economy->AddCurrency(ECurrencyType::Fish, FishStoredAmount);
	FishStoredAmount = 0.0;
}

void AShip_Base::HandleLoopStateChanged(ELoopState OldState, ELoopState NewState)
{
	bInFishingLoop = (NewState == ELoopState::Fishing);

	if (NewState == ELoopState::Village)
	{
		TransferFishToEconomy();
	}
}

void AShip_Base::HandleUpgradeStatChanged(FGameplayTag StatTag)
{
	if (!ShipDefinition) return;

	// Only recalculate when an upgrade actually affects one of this ship's stats
	if (StatTag == ShipDefinition->SpeedStatTag         ||
		StatTag == ShipDefinition->FishingRadiusStatTag  ||
		StatTag == ShipDefinition->FishingCooldownStatTag)
	{
		RefreshStatsFromRegistry();
	}
}
