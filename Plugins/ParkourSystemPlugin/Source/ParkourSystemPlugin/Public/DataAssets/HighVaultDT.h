// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParkourVariables.h"
#include "HighVaultDT.generated.h"

/**
 * Data table containing variables for hight vault action, inheriting from UParkourVariables.
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API UHighVaultDT : public UParkourVariables
{
	GENERATED_BODY()

public:
	UHighVaultDT();
};
