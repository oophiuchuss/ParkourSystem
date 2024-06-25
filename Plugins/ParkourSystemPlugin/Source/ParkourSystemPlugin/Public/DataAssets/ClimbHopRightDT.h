// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParkourVariables.h"
#include "ClimbHopRightDT.generated.h"

/**
 * Data table containing variables for braced climb hop right action, inheriting from UParkourVariables.
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API UClimbHopRightDT : public UParkourVariables
{
	GENERATED_BODY()
	
public:
	UClimbHopRightDT();
};
