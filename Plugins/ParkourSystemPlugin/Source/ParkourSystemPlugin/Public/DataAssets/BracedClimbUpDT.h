// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParkourVariables.h"
#include "BracedClimbUpDT.generated.h"

/**
 * Data table containing variables for braced climb up action, inheriting from UParkourVariables.
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API UBracedClimbUpDT : public UParkourVariables
{
	GENERATED_BODY()
	
public:
	UBracedClimbUpDT();
};
