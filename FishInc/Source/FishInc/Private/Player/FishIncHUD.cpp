// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/FishIncHUD.h"
#include "Player/FishIncPlayerController.h"
#include "Engine/Canvas.h"

static FString FormatCurrency(double Value)
{
	if (Value >= 1.0e12) return FString::Printf(TEXT("%.3ft"), Value / 1.0e12);
	if (Value >= 1.0e9)  return FString::Printf(TEXT("%.3fb"), Value / 1.0e9);
	if (Value >= 1.0e6)  return FString::Printf(TEXT("%.3fm"), Value / 1.0e6);
	if (Value >= 1.0e3)  return FString::Printf(TEXT("%.3fk"), Value / 1.0e3);
	return FString::Printf(TEXT("%.0f"), Value);
}

void AFishIncHUD::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* GI = GetGameInstance();
	ensureMsgf(GI, TEXT("AFishIncHUD::BeginPlay — GameInstance is null"));
	if (GI)
	{
		UEconomySubsystem* Economy = GI->GetSubsystem<UEconomySubsystem>();
		ensureMsgf(Economy, TEXT("AFishIncHUD::BeginPlay — UEconomySubsystem not found"));
		if (Economy)
		{
			Economy->OnCurrencyChanged.AddDynamic(this, &AFishIncHUD::HandleCurrencyChanged);

			CachedFish   = Economy->GetCurrencyAmount(ECurrencyType::Fish);
			CachedCoins  = Economy->GetCurrencyAmount(ECurrencyType::Coins);
			CachedPearls = Economy->GetCurrencyAmount(ECurrencyType::Pearls);
		}
	}

	AFishIncPlayerController* FishPC = Cast<AFishIncPlayerController>(PlayerOwner);
	ensureMsgf(FishPC, TEXT("AFishIncHUD::BeginPlay — Expected AFishIncPlayerController"));
	if (FishPC)
	{
		FishPC->OnBuildingSelected.AddUObject(this, &AFishIncHUD::HandleBuildingSelected);
	}

	if (LoopToggleWidgetClass && PlayerOwner)
	{
		LoopToggleWidgetInstance = CreateWidget<ULoopToggleWidget>(PlayerOwner, LoopToggleWidgetClass);
		if (LoopToggleWidgetInstance)
		{
			LoopToggleWidgetInstance->AddToViewport();
		}
	}

	if (BuildingShopWidgetClass && PlayerOwner)
	{
		BuildingShopWidgetInstance = CreateWidget<UBuildingShopWidget>(PlayerOwner, BuildingShopWidgetClass);
		if (BuildingShopWidgetInstance)
		{
			BuildingShopWidgetInstance->AddToViewport();
		}
	}

	if (BuildingUpgradeWidgetClass && PlayerOwner)
	{
		BuildingUpgradeWidgetInstance = CreateWidget<UBuildingUpgradeWidget>(PlayerOwner, BuildingUpgradeWidgetClass);
		if (BuildingUpgradeWidgetInstance)
		{
			BuildingUpgradeWidgetInstance->AddToViewport();
			BuildingUpgradeWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void AFishIncHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AFishIncPlayerController* FishPC = Cast<AFishIncPlayerController>(PlayerOwner))
	{
		FishPC->OnBuildingSelected.RemoveAll(this);
	}

	if (LoopToggleWidgetInstance)
	{
		LoopToggleWidgetInstance->RemoveFromParent();
		LoopToggleWidgetInstance = nullptr;
	}

	if (BuildingShopWidgetInstance)
	{
		BuildingShopWidgetInstance->RemoveFromParent();
		BuildingShopWidgetInstance = nullptr;
	}

	if (BuildingUpgradeWidgetInstance)
	{
		BuildingUpgradeWidgetInstance->RemoveFromParent();
		BuildingUpgradeWidgetInstance = nullptr;
	}

	UGameInstance* GI = GetGameInstance();
	if (GI)
	{
		if (UEconomySubsystem* Economy = GI->GetSubsystem<UEconomySubsystem>())
		{
			Economy->OnCurrencyChanged.RemoveAll(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void AFishIncHUD::HandleBuildingSelected(ABuilding_Base* Building)
{
	if (!BuildingUpgradeWidgetInstance) return;

	if (!Building)
	{
		BuildingUpgradeWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	BuildingUpgradeWidgetInstance->SetBuilding(Building);
	BuildingUpgradeWidgetInstance->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void AFishIncHUD::HandleCurrencyChanged(ECurrencyType CurrencyType, double NewAmount)
{
	switch (CurrencyType)
	{
	case ECurrencyType::Fish:   CachedFish   = NewAmount; break;
	case ECurrencyType::Coins:  CachedCoins  = NewAmount; break;
	case ECurrencyType::Pearls: CachedPearls = NewAmount; break;
	}
}

void AFishIncHUD::DrawHUD()
{
	Super::DrawHUD();

	UFont* Font = HUDFont.Get() ? HUDFont.Get() : GEngine->GetMediumFont();
	const float LineHeight = FontScale * 16.f;

	DrawText(FString::Printf(TEXT("Fish:   %s"), *FormatCurrency(CachedFish)),   FLinearColor(0.4f, 0.9f, 0.4f, 1.f), CounterPadding.X, CounterPadding.Y,                   Font, FontScale);
	DrawText(FString::Printf(TEXT("Coins:  %s"), *FormatCurrency(CachedCoins)),  FLinearColor::Yellow,                  CounterPadding.X, CounterPadding.Y + LineHeight,       Font, FontScale);
	DrawText(FString::Printf(TEXT("Pearls: %s"), *FormatCurrency(CachedPearls)), FLinearColor(0.85f, 0.6f, 1.f, 1.f), CounterPadding.X, CounterPadding.Y + LineHeight * 2.f, Font, FontScale);
}
