// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParkourVariables.h"
#include "ClimbHopLeftUpDT.generated.h"

/**
 * Data table containing variables for braced climb hop left action, inheriting from UParkourVariables.
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API UClimbHopLeftUpDT : public UParkourVariables
{
	GENERATED_BODY()
	
public:
	UClimbHopLeftUpDT();
};
