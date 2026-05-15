// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Subsystems/LoopStateSubsystem.h"
#include "LoopToggleWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class FISHINC_API ULoopToggleWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	// Button named exactly "LoopToggleButton" must exist in the Blueprint widget.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> LoopToggleButton;

	// Optional label that shows "Go to Village" or "Start Fishing".
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> LoopStateLabel;

	UFUNCTION()
	void HandleButtonClicked();

	void HandleLoopStateChanged(ELoopState OldState, ELoopState NewState);
	void RefreshLabel();
};
