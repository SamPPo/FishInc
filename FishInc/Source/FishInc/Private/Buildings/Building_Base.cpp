// Fill out your copyright notice in the Description page of Project Settings.

#include "Buildings/Building_Base.h"
#include "Buildings/BuildingDefinition.h"
#include "Subsystems/UpgradeRegistry.h"
#include "Subsystems/EconomySubsystem.h"
#include "Workers/Worker_Base.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/BuildingUpgradeWidget.h"

ABuilding_Base::ABuilding_Base()
{
	PrimaryActorTick.bCanEverTick = false;

	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BuildingMesh"));
	SetRootComponent(BuildingMesh);

	UpgradeWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("UpgradeWidgetComponent"));
	UpgradeWidgetComponent->SetupAttachment(RootComponent);
	UpgradeWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	UpgradeWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 150.f));
	UpgradeWidgetComponent->SetDrawSize(FVector2D(300.f, 200.f));
	UpgradeWidgetComponent->SetVisibility(false);
	// Prevent the widget component's world-space collision from intercepting cursor traces on the building mesh.
	//UpgradeWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABuilding_Base::BeginPlay()
{
	Super::BeginPlay();

	OnBeginCursorOver.AddDynamic(this, &ABuilding_Base::HandleActorBeginCursorOver);
	OnEndCursorOver.AddDynamic(this, &ABuilding_Base::HandleActorEndCursorOver);

	if (UpgradeWidgetComponent)
	{
		if (UBuildingUpgradeWidget* Widget = Cast<UBuildingUpgradeWidget>(UpgradeWidgetComponent->GetWidget()))
		{
			Widget->SetBuilding(this);
		}
	}
}

void ABuilding_Base::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(HideWidgetTimerHandle);

	if (!BuildingInstanceID.IsNone())
	{
		UGameInstance* GI = GetGameInstance();
		if (GI)
		{
			if (UUpgradeRegistry* Registry = GI->GetSubsystem<UUpgradeRegistry>())
			{
				Registry->UnregisterContributions(BuildingInstanceID);
			}
		}
	}

	Super::EndPlay(EndPlayReason);
}

void ABuilding_Base::InitFromDefinition(UBuildingDefinition* InDefinition)
{
	if (!InDefinition) return;

	Definition = InDefinition;
	BuildingInstanceID = FName(*FGuid::NewGuid().ToString());
	BuildingLevel = 1;

	RefreshRegistryContributions();

	if (UpgradeWidgetComponent && UpgradeWidgetClass)
	{
		UpgradeWidgetComponent->SetWidgetClass(UpgradeWidgetClass);
	}
}

bool ABuilding_Base::PurchaseUpgrade()
{
	if (!Definition || IsAtMaxLevel()) return false;

	UGameInstance* GI = GetGameInstance();
	ensureMsgf(GI, TEXT("ABuilding_Base::PurchaseUpgrade — GameInstance is null"));
	if (!GI) return false;

	UEconomySubsystem* Economy = GI->GetSubsystem<UEconomySubsystem>();
	ensureMsgf(Economy, TEXT("ABuilding_Base::PurchaseUpgrade — UEconomySubsystem not found"));
	if (!Economy) return false;

	if (!Economy->Spend(ECurrencyType::Coins, GetUpgradeCost())) return false;

	BuildingLevel++;
	RefreshRegistryContributions();
	return true;
}

double ABuilding_Base::GetUpgradeCost() const
{
	if (!Definition || IsAtMaxLevel()) return 0.0;
	return Definition->GetUpgradeCostAtLevel(BuildingLevel);
}

bool ABuilding_Base::CanUpgrade() const
{
	if (!Definition || IsAtMaxLevel()) return false;

	UGameInstance* GI = GetGameInstance();
	if (!GI) return false;

	const UEconomySubsystem* Economy = GI->GetSubsystem<UEconomySubsystem>();
	return Economy && Economy->CanAfford(ECurrencyType::Coins, GetUpgradeCost());
}

bool ABuilding_Base::IsAtMaxLevel() const
{
	if (!Definition) return true;
	return BuildingLevel >= Definition->MaxUpgradeLevel;
}

void ABuilding_Base::RefreshRegistryContributions()
{
	if (!Definition || BuildingInstanceID.IsNone()) return;

	UGameInstance* GI = GetGameInstance();
	ensureMsgf(GI, TEXT("ABuilding_Base::RefreshRegistryContributions — GameInstance is null"));
	if (!GI) return;

	UUpgradeRegistry* Registry = GI->GetSubsystem<UUpgradeRegistry>();
	ensureMsgf(Registry, TEXT("ABuilding_Base::RefreshRegistryContributions — UUpgradeRegistry not found"));
	if (!Registry) return;

	TArray<FStatContribution> Contributions;
	for (const FBuildingUpgrade& Upgrade : Definition->Upgrades)
	{
		if (!Upgrade.AffectedStat.IsValid()) continue;

		FStatContribution Contrib;
		Contrib.StatTag     = Upgrade.AffectedStat;
		Contrib.UpgradeType = Upgrade.UpgradeType;
		Contrib.Value       = Upgrade.GetValueAtLevel(BuildingLevel);
		Contributions.Add(Contrib);
	}

	Registry->RegisterContributions(BuildingInstanceID, Contributions);
}

void ABuilding_Base::Interact_Implementation()
{
	OnActivated();
}

void ABuilding_Base::OnActivated()
{
	OnBuildingActivated.Broadcast(this);
}

bool ABuilding_Base::AssignWorker(AWorker_Base* Worker)
{
	if (!Worker || Workers.Num() >= MaxWorkers || Workers.Contains(Worker)) return false;
	Workers.Add(Worker);
	return true;
}

bool ABuilding_Base::RemoveWorker(AWorker_Base* Worker)
{
	return Workers.Remove(Worker) > 0;
}

bool ABuilding_Base::HasWorkerSlotAvailable() const
{
	return Workers.Num() < MaxWorkers;
}

void ABuilding_Base::ShowUpgradeWidget()
{
	GetWorldTimerManager().ClearTimer(HideWidgetTimerHandle);
	if (UpgradeWidgetComponent && !UpgradeWidgetComponent->IsVisible())
	{
		UpgradeWidgetComponent->SetVisibility(true);
	}
}

void ABuilding_Base::HideUpgradeWidget()
{
	if (UpgradeWidgetComponent)
	{
		UpgradeWidgetComponent->SetVisibility(false);
	}
}

void ABuilding_Base::NotifyWidgetHoverBegin()
{
	GetWorldTimerManager().ClearTimer(HideWidgetTimerHandle);
}

void ABuilding_Base::NotifyWidgetHoverEnd()
{
	GetWorldTimerManager().SetTimer(HideWidgetTimerHandle, this, &ABuilding_Base::HideUpgradeWidget, 0.5f, false);
}

void ABuilding_Base::HandleActorBeginCursorOver(AActor* TouchedActor)
{
	ShowUpgradeWidget();
}

void ABuilding_Base::HandleActorEndCursorOver(AActor* TouchedActor)
{
	GetWorldTimerManager().SetTimer(HideWidgetTimerHandle, this, &ABuilding_Base::HideUpgradeWidget, 0.5f, false);
}
