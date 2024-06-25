// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParkourVariables.h"
#include "ClimbHopUpDT.generated.h"

/**
 * Data table containing variables for braced climb hop up action, inheriting from UParkourVariables.
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API UClimbHopUpDT : public UParkourVariables
{
	GENERATED_BODY()
	
public:
	UClimbHopUpDT();
};
