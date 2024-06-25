// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParkourVariables.h"
#include "MantleDT.generated.h"

/**
 * Data table containing variables for standard mantle action, inheriting from UParkourVariables.
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API UMantleDT : public UParkourVariables
{
	GENERATED_BODY()
	

public:
	UMantleDT();
};
