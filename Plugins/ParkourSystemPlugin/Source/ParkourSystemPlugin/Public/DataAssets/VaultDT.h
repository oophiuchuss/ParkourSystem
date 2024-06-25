// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParkourVariables.h"
#include "VaultDT.generated.h"

/**
 * Data table containing variables for standard vault action, inheriting from UParkourVariables.
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API UVaultDT : public UParkourVariables
{
	GENERATED_BODY()
	
public:
	UVaultDT();
};
