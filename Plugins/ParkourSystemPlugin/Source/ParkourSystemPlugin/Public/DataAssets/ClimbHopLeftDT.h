// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParkourVariables.h"
#include "ClimbHopLeftDT.generated.h"

/**
 * Data table containing variables for braced climb action, inheriting from UParkourVariables.
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API UClimbHopLeftDT : public UParkourVariables
{
	GENERATED_BODY()
	
public:
	UClimbHopLeftDT();
};
