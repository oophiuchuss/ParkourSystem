// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParkourVariables.h"
#include "ClimbHopDownDT.generated.h"

/**
 * Data table containing variables for braced climb hop down action, inheriting from UParkourVariables.
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API UClimbHopDownDT : public UParkourVariables
{
	GENERATED_BODY()
	
public:
	UClimbHopDownDT();
};
