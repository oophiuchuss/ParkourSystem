// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParkourVariables.h"
#include "BracedDropDownDT.generated.h"

/**
 * Data table containing variables for braced drop down action, inheriting from UParkourVariables.
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API UBracedDropDownDT : public UParkourVariables
{
	GENERATED_BODY()

public:
	UBracedDropDownDT();
};
