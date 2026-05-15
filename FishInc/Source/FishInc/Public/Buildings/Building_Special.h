// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buildings/Building_Base.h"
#include "Building_Special.generated.h"

// Blueprint subclasses of ABuilding_Special should override Interact_Implementation
// to define custom activation logic. Call Super to preserve the OnBuildingActivated broadcast.

UCLASS(Blueprintable)
class FISHINC_API ABuilding_Special : public ABuilding_Base
{
	GENERATED_BODY()

public:
	ABuilding_Special();

protected:
	virtual void OnActivated() override;
};
