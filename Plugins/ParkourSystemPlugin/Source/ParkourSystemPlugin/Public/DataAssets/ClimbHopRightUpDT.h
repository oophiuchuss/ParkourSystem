// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParkourVariables.h"
#include "ClimbHopRightUpDT.generated.h"

/**
 * Data table containing variables for braced climb hop right up action, inheriting from UParkourVariables.
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API UClimbHopRightUpDT : public UParkourVariables
{
	GENERATED_BODY()
	
public:
	UClimbHopRightUpDT();
};
