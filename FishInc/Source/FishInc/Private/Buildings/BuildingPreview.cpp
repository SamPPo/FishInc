// Fill out your copyright notice in the Description page of Project Settings.

#include "Buildings/BuildingPreview.h"
#include "Player/FishIncPlayerController.h"
#include "Components/StaticMeshComponent.h"

ABuildingPreview::ABuildingPreview()
{
	PrimaryActorTick.bCanEverTick = false;

	PreviewMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMesh"));
	PreviewMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(PreviewMeshComponent);
}

void ABuildingPreview::BeginPlay()
{
	Super::BeginPlay();

	if (AFishIncPlayerController* PC = Cast<AFishIncPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		PC->OnCursorMoved.AddUObject(this, &ABuildingPreview::HandleCursorMoved);
	}
}

void ABuildingPreview::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AFishIncPlayerController* PC = Cast<AFishIncPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		PC->OnCursorMoved.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void ABuildingPreview::SetPreviewMesh(UStaticMesh* Mesh)
{
	PreviewMeshComponent->SetStaticMesh(Mesh);
}

void ABuildingPreview::HandleCursorMoved(FVector WorldPosition)
{
	SetActorLocation(WorldPosition);
}
