// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParkourVariables.h"
#include "LowMantleDT.generated.h"

/**
 * Data table containing variables for low mantle action, inheriting from UParkourVariables.
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API ULowMantleDT : public UParkourVariables
{
	GENERATED_BODY()
	
public:
	ULowMantleDT();
};
