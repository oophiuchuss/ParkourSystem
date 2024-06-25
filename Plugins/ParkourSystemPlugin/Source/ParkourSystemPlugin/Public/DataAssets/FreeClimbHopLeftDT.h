// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParkourVariables.h"
#include "FreeClimbHopLeftDT.generated.h"

/**
 * Data table containing variables for free hang hop left action, inheriting from UParkourVariables.
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API UFreeClimbHopLeftDT : public UParkourVariables
{
	GENERATED_BODY()
	
public:
	UFreeClimbHopLeftDT();
};
