// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/LoopStateSubsystem.h"

void ULoopStateSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void ULoopStateSubsystem::SetLoopState(ELoopState NewState)
{
	if (CurrentState == NewState) return;

	const ELoopState OldState = CurrentState;
	CurrentState = NewState;
	OnLoopStateChanged.Broadcast(OldState, NewState);
}
