// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/LoopToggleWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void ULoopToggleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	LoopToggleButton->OnClicked.AddDynamic(this, &ULoopToggleWidget::HandleButtonClicked);

	UGameInstance* GI = GetGameInstance();
	ensureMsgf(GI, TEXT("ULoopToggleWidget::NativeConstruct — GameInstance is null"));
	if (GI)
	{
		ULoopStateSubsystem* LoopState = GI->GetSubsystem<ULoopStateSubsystem>();
		ensureMsgf(LoopState, TEXT("ULoopToggleWidget::NativeConstruct — ULoopStateSubsystem not found"));
		if (LoopState)
		{
			LoopState->OnLoopStateChanged.AddUObject(this, &ULoopToggleWidget::HandleLoopStateChanged);
		}
	}

	RefreshLabel();
}

void ULoopToggleWidget::NativeDestruct()
{
	LoopToggleButton->OnClicked.RemoveAll(this);

	UGameInstance* GI = GetGameInstance();
	if (GI)
	{
		if (ULoopStateSubsystem* LoopState = GI->GetSubsystem<ULoopStateSubsystem>())
		{
			LoopState->OnLoopStateChanged.RemoveAll(this);
		}
	}

	Super::NativeDestruct();
}

void ULoopToggleWidget::HandleButtonClicked()
{
	UGameInstance* GI = GetGameInstance();
	ensureMsgf(GI, TEXT("ULoopToggleWidget::HandleButtonClicked — GameInstance is null"));
	if (!GI) return;

	ULoopStateSubsystem* LoopState = GI->GetSubsystem<ULoopStateSubsystem>();
	ensureMsgf(LoopState, TEXT("ULoopToggleWidget::HandleButtonClicked — ULoopStateSubsystem not found"));
	if (!LoopState) return;

	const ELoopState NewState = (LoopState->GetLoopState() == ELoopState::Fishing)
		? ELoopState::Village
		: ELoopState::Fishing;
	LoopState->SetLoopState(NewState);
}

void ULoopToggleWidget::HandleLoopStateChanged(ELoopState /*OldState*/, ELoopState /*NewState*/)
{
	RefreshLabel();
}

void ULoopToggleWidget::RefreshLabel()
{
	if (!LoopStateLabel) return;

	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	ULoopStateSubsystem* LoopState = GI->GetSubsystem<ULoopStateSubsystem>();
	if (!LoopState) return;

	const FText Label = (LoopState->GetLoopState() == ELoopState::Fishing)
		? FText::FromString(TEXT("Go to Village"))
		: FText::FromString(TEXT("Start Fishing"));
	LoopStateLabel->SetText(Label);
}
