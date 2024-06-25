// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParkourVariables.h"
#include "ThinVaultDT.generated.h"

/**
 * Data table containing variables for thing vault action, inheriting from UParkourVariables.
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API UThinVaultDT : public UParkourVariables
{
	GENERATED_BODY()

public:
	UThinVaultDT();
};
