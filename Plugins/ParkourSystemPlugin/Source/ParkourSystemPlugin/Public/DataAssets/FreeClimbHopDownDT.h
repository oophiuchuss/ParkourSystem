// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParkourVariables.h"
#include "FreeClimbHopDownDT.generated.h"

/**
 * Data table containing variables for free hang hop down action, inheriting from UParkourVariables.
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API UFreeClimbHopDownDT : public UParkourVariables
{
	GENERATED_BODY()
	
public:
	UFreeClimbHopDownDT();
};
