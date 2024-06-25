// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParkourVariables.h"
#include "FreeClimbHopRightDT.generated.h"

/**
 * Data table containing variables for free hang hop right action, inheriting from UParkourVariables.
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API UFreeClimbHopRightDT : public UParkourVariables
{
	GENERATED_BODY()
	
public:
	UFreeClimbHopRightDT();
};
