// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildingPreview.generated.h"

UCLASS()
class FISHINC_API ABuildingPreview : public AActor
{
	GENERATED_BODY()

public:
	ABuildingPreview();

	void SetPreviewMesh(UStaticMesh* Mesh);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building",
		meta = (Tooltip = "Mesh displayed at the cursor position while the player is choosing where to place a building."))
	TObjectPtr<UStaticMeshComponent> PreviewMeshComponent;

private:
	void HandleCursorMoved(FVector WorldPosition);
};
