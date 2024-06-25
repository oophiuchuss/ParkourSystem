// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParkourVariables.h"
#include "FreeHangDropDownDT.generated.h"

/**
 * Data table containing variables for free hang drop down action, inheriting from UParkourVariables.
 */ 
UCLASS()
class PARKOURSYSTEMPLUGIN_API UFreeHangDropDownDT : public UParkourVariables
{
	GENERATED_BODY()

public:
	UFreeHangDropDownDT();
};
