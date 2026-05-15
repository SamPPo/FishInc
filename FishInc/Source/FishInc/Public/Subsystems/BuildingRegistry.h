// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BuildingRegistry.generated.h"

class UBuildingDefinition;

UCLASS()
class FISHINC_API UBuildingRegistry : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	bool AreDefinitionsLoaded() const { return bDefinitionsLoaded; }

	// Returns all loaded building definitions. Empty until OnDefinitionsLoaded fires.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Building",
		meta = (Tooltip = "Returns all loaded building definitions. Bind to OnDefinitionsLoaded before calling this."))
	TArray<UBuildingDefinition*> GetAllDefinitions() const;

	DECLARE_MULTICAST_DELEGATE(FOnBuildingDefinitionsLoaded)
	FOnBuildingDefinitionsLoaded OnDefinitionsLoaded;

private:
	void RequestDefinitionsLoad();
	void OnDefinitionsLoadCompleted();

	bool bDefinitionsLoaded = false;

	UPROPERTY()
	TArray<TObjectPtr<UBuildingDefinition>> Definitions;
};
