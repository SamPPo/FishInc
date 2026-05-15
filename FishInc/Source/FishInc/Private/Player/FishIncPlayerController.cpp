// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/FishIncPlayerController.h"
#include "Player/FishIncCheatManager.h"
#include "Interfaces/Interactable.h"
#include "Buildings/BuildingDefinition.h"
#include "Buildings/BuildingPreview.h"
#include "Buildings/Building_Base.h"
#include "Subsystems/EconomySubsystem.h"
#include "Kismet/GameplayStatics.h"

AFishIncPlayerController::AFishIncPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	CheatClass = UFishIncCheatManager::StaticClass();
}

void AFishIncPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	bEnableMouseOverEvents = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}

void AFishIncPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CursorWorld;
	if (GetCursorFloorPosition(CursorWorld))
	{
		LastCursorWorldPosition = CursorWorld;
	}

	OnCursorMoved.Broadcast(LastCursorWorldPosition);
}

void AFishIncPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindKey(EKeys::LeftMouseButton,  IE_Pressed, this, &AFishIncPlayerController::OnInteractPressed);
	InputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &AFishIncPlayerController::OnCancelPressed);
}

void AFishIncPlayerController::EnterPlacementMode(UBuildingDefinition* Definition)
{
	if (!Definition) return;

	if (bInPlacementMode)
	{
		ExitPlacementMode();
	}

	bInPlacementMode = true;
	PendingBuildingDef = Definition;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	PreviewActor = GetWorld()->SpawnActor<ABuildingPreview>(ABuildingPreview::StaticClass(), FTransform(LastCursorWorldPosition), Params);
	if (PreviewActor)
	{
		PreviewActor->SetPreviewMesh(Definition->PreviewMesh.Get());
	}
}

void AFishIncPlayerController::ExitPlacementMode()
{
	bInPlacementMode = false;
	PendingBuildingDef = nullptr;

	if (PreviewActor)
	{
		PreviewActor->Destroy();
		PreviewActor = nullptr;
	}
}

void AFishIncPlayerController::OnInteractPressed()
{
	if (bInPlacementMode)
	{
		ConfirmPlacement();
		return;
	}

	FHitResult Hit;
	if (!GetCursorInteractable(Hit))
	{
		OnBuildingSelected.Broadcast(nullptr);
		return;
	}

	AActor* Actor = Hit.GetActor();
	if (Actor && Actor->Implements<UInteractable>())
	{
		IInteractable::Execute_Interact(Actor);
	}

	OnBuildingSelected.Broadcast(Cast<ABuilding_Base>(Actor));
}

void AFishIncPlayerController::OnCancelPressed()
{
	if (bInPlacementMode)
	{
		ExitPlacementMode();
	}
}

void AFishIncPlayerController::ConfirmPlacement()
{
	if (!PendingBuildingDef || !PendingBuildingDef->BuildingClass) return;

	UGameInstance* GI = GetGameInstance();
	ensureMsgf(GI, TEXT("AFishIncPlayerController::ConfirmPlacement — GameInstance is null"));
	if (!GI) return;

	UEconomySubsystem* Economy = GI->GetSubsystem<UEconomySubsystem>();
	ensureMsgf(Economy, TEXT("AFishIncPlayerController::ConfirmPlacement — UEconomySubsystem not found"));
	if (!Economy) return;

	// Buy — deduct cost before placing. Abort if the player can't afford it.
	if (!Economy->Spend(ECurrencyType::Coins, PendingBuildingDef->PurchaseCost)) return;

	// Place — deferred spawn so InitFromDefinition runs before BeginPlay.
	const FTransform SpawnTransform(LastCursorWorldPosition);
	ABuilding_Base* Building = GetWorld()->SpawnActorDeferred<ABuilding_Base>(
		PendingBuildingDef->BuildingClass,
		SpawnTransform,
		nullptr, nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	if (Building)
	{
		Building->InitFromDefinition(PendingBuildingDef);
		UGameplayStatics::FinishSpawningActor(Building, SpawnTransform);
	}

	ExitPlacementMode();
}

bool AFishIncPlayerController::GetCursorInteractable(FHitResult& OutHit) const
{
	float MouseX, MouseY;
	if (!GetMousePosition(MouseX, MouseY)) return false;

	FVector WorldLoc, WorldDir;
	if (!DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLoc, WorldDir)) return false;

	const FVector TraceEnd = WorldLoc + WorldDir * 100000.f;
	return GetWorld()->LineTraceSingleByChannel(OutHit, WorldLoc, TraceEnd, ECC_Visibility);
}

bool AFishIncPlayerController::GetCursorFloorPosition(FVector& OutPosition) const
{
	float MouseX, MouseY;
	if (!GetMousePosition(MouseX, MouseY)) return false;

	FVector WorldLoc, WorldDir;
	if (!DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLoc, WorldDir)) return false;

	FHitResult Hit;
	const FVector TraceEnd = WorldLoc + WorldDir * 100000.f;
	if (GetWorld()->LineTraceSingleByChannel(Hit, WorldLoc, TraceEnd, ECC_GameTraceChannel1))
	{
		OutPosition = Hit.Location;
		return true;
	}
	return false;
}
