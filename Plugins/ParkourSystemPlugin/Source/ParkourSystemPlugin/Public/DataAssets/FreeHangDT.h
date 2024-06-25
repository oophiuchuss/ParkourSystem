// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParkourVariables.h"
#include "FreeHangDT.generated.h"

/**
 * Data table containing variables for free hang climb action, inheriting from UParkourVariables.
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API UFreeHangDT : public UParkourVariables
{
	GENERATED_BODY()

public:
	UFreeHangDT();
};
