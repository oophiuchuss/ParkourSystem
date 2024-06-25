// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParkourVariables.h"
#include "FallingBracedClimbDT.generated.h"

/**
 * Data table containing variables for falling braced climb action, inheriting from UParkourVariables.
 */ 
UCLASS()
class PARKOURSYSTEMPLUGIN_API UFallingBracedClimbDT : public UParkourVariables
{
	GENERATED_BODY()
	
public:
	UFallingBracedClimbDT();
};
