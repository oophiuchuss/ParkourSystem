// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParkourVariables.h"
#include "FreeHangClimbUpDT.generated.h"

/**
 * Data table containing variables for free hang climb up action, inheriting from UParkourVariables.
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API UFreeHangClimbUpDT : public UParkourVariables
{
	GENERATED_BODY()
	
public:
	UFreeHangClimbUpDT();
};
