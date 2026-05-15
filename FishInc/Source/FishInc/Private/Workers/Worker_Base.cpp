// Fill out your copyright notice in the Description page of Project Settings.

#include "Workers/Worker_Base.h"
#include "Buildings/Building_Base.h"
#include "Interfaces/Interactable.h"
#include "Subsystems/UpgradeRegistry.h"
#include "Components/StaticMeshComponent.h"

AWorker_Base::AWorker_Base()
{
	PrimaryActorTick.bCanEverTick = false;

	WorkerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WorkerMesh"));
	SetRootComponent(WorkerMesh);
}

void AWorker_Base::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* GI = GetGameInstance();
	ensureMsgf(GI, TEXT("AWorker_Base::BeginPlay — GameInstance is null"));
	if (GI)
	{
		UUpgradeRegistry* Registry = GI->GetSubsystem<UUpgradeRegistry>();
		ensureMsgf(Registry, TEXT("AWorker_Base::BeginPlay — UUpgradeRegistry not found"));
		if (Registry)
		{
			Registry->OnStatChanged.AddUObject(this, &AWorker_Base::HandleUpgradeStatChanged);
		}
	}

	if (AssignedBuilding)
	{
		RefreshAutoActivateInterval();
	}
}

void AWorker_Base::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(AutoActivateTimerHandle);

	if (AssignedBuilding)
	{
		AssignedBuilding->RemoveWorker(this);
	}

	UGameInstance* GI = GetGameInstance();
	if (GI)
	{
		if (UUpgradeRegistry* Registry = GI->GetSubsystem<UUpgradeRegistry>())
		{
			Registry->OnStatChanged.RemoveAll(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void AWorker_Base::AssignToBuilding(ABuilding_Base* NewBuilding)
{
	if (AssignedBuilding)
	{
		AssignedBuilding->RemoveWorker(this);
		GetWorldTimerManager().ClearTimer(AutoActivateTimerHandle);
		AssignedBuilding = nullptr;
	}

	if (!NewBuilding) return;

	if (NewBuilding->AssignWorker(this))
	{
		AssignedBuilding = NewBuilding;
		RefreshAutoActivateInterval();
	}
}

void AWorker_Base::AutoActivate()
{
	if (!AssignedBuilding) return;
	IInteractable::Execute_Interact(AssignedBuilding.Get());
}

void AWorker_Base::RefreshAutoActivateInterval()
{
	float Interval = AutoActivateInterval;

	if (AutoActivateIntervalStatTag.IsValid())
	{
		UGameInstance* GI = GetGameInstance();
		ensureMsgf(GI, TEXT("AWorker_Base::RefreshAutoActivateInterval — GameInstance is null"));
		if (GI)
		{
			UUpgradeRegistry* Registry = GI->GetSubsystem<UUpgradeRegistry>();
			ensureMsgf(Registry, TEXT("AWorker_Base::RefreshAutoActivateInterval — UUpgradeRegistry not found"));
			if (Registry)
			{
				Interval += Registry->ResolveStat(AutoActivateIntervalStatTag);
			}
		}
	}

	GetWorldTimerManager().SetTimer(
		AutoActivateTimerHandle,
		this, &AWorker_Base::AutoActivate,
		FMath::Max(0.1f, Interval),
		true);
}

void AWorker_Base::HandleUpgradeStatChanged(FGameplayTag StatTag)
{
	if (AutoActivateIntervalStatTag.IsValid() && StatTag == AutoActivateIntervalStatTag)
	{
		RefreshAutoActivateInterval();
	}
}
