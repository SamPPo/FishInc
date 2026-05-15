// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/PlayerPawn_Base.h"

APlayerPawn_Base::APlayerPawn_Base()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlayerPawn_Base::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* GI = GetGameInstance();
	ensureMsgf(GI, TEXT("APlayerPawn_Base::BeginPlay — GameInstance is null"));
	if (GI)
	{
		ULoopStateSubsystem* LoopState = GI->GetSubsystem<ULoopStateSubsystem>();
		ensureMsgf(LoopState, TEXT("APlayerPawn_Base::BeginPlay — ULoopStateSubsystem not found"));
		if (LoopState)
		{
			LoopState->OnLoopStateChanged.AddUObject(this, &APlayerPawn_Base::HandleLoopStateChanged);
		}
	}
}

void APlayerPawn_Base::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameInstance* GI = GetGameInstance();
	if (GI)
	{
		if (ULoopStateSubsystem* LoopState = GI->GetSubsystem<ULoopStateSubsystem>())
		{
			LoopState->OnLoopStateChanged.RemoveAll(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void APlayerPawn_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerPawn_Base::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APlayerPawn_Base::HandleLoopStateChanged(ELoopState OldState, ELoopState NewState)
{
	OnLoopStateChanged(OldState, NewState);
}
