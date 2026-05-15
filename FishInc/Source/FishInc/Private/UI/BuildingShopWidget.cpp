// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/BuildingShopWidget.h"
#include "Buildings/BuildingDefinition.h"
#include "Player/FishIncPlayerController.h"
#include "Subsystems/BuildingRegistry.h"

void UBuildingShopWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGameInstance* GI = GetGameInstance();
	ensureMsgf(GI, TEXT("UBuildingShopWidget::NativeConstruct — GameInstance is null"));
	if (!GI) return;

	UBuildingRegistry* Registry = GI->GetSubsystem<UBuildingRegistry>();
	ensureMsgf(Registry, TEXT("UBuildingShopWidget::NativeConstruct — UBuildingRegistry not found"));
	if (!Registry) return;

	if (Registry->AreDefinitionsLoaded())
	{
		OnShopBuilt();
	}
	else
	{
		Registry->OnDefinitionsLoaded.AddUObject(this, &UBuildingShopWidget::HandleDefinitionsLoaded);
	}
}

void UBuildingShopWidget::NativeDestruct()
{
	UGameInstance* GI = GetGameInstance();
	if (GI)
	{
		if (UBuildingRegistry* Registry = GI->GetSubsystem<UBuildingRegistry>())
		{
			Registry->OnDefinitionsLoaded.RemoveAll(this);
		}
	}

	Super::NativeDestruct();
}

void UBuildingShopWidget::HandleDefinitionsLoaded()
{
	UGameInstance* GI = GetGameInstance();
	if (GI)
	{
		if (UBuildingRegistry* Registry = GI->GetSubsystem<UBuildingRegistry>())
		{
			Registry->OnDefinitionsLoaded.RemoveAll(this);
		}
	}

	OnShopBuilt();
}

TArray<UBuildingDefinition*> UBuildingShopWidget::GetAvailableBuildings() const
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return {};

	UBuildingRegistry* Registry = GI->GetSubsystem<UBuildingRegistry>();
	if (!Registry) return {};

	return Registry->GetAllDefinitions();
}

void UBuildingShopWidget::BuyBuilding(UBuildingDefinition* Definition)
{
	if (!Definition) return;

	APlayerController* PC = GetOwningPlayer();
	ensureMsgf(PC, TEXT("UBuildingShopWidget::BuyBuilding — OwningPlayer is null"));
	if (!PC) return;

	AFishIncPlayerController* FishPC = Cast<AFishIncPlayerController>(PC);
	ensureMsgf(FishPC, TEXT("UBuildingShopWidget::BuyBuilding — Expected AFishIncPlayerController"));
	if (!FishPC) return;

	FishPC->EnterPlacementMode(Definition);
}
