// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParkourVariables.h"
#include "FallingFreeHangClimbDT.generated.h"

/**
 * Data table containing variables for follaing free hang climb action, inheriting from UParkourVariables.
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API UFallingFreeHangClimbDT : public UParkourVariables
{
	GENERATED_BODY()
	
public:
	UFallingFreeHangClimbDT();
};
