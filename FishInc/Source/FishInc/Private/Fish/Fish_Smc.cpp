// Fill out your copyright notice in the Description page of Project Settings.

#include "FishInc/Public/Fish/Fish_Smc.h"

UFish_Smc::UFish_Smc()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFish_Smc::BeginPlay()
{
	Super::BeginPlay();
}

void UFish_Smc::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
