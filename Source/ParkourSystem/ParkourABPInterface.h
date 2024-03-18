// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ParkourABPInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UParkourABPInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PARKOURSYSTEM_API IParkourABPInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	bool SetParkourState(const FGameplayTag& NewParkourState);

	bool SetParkourAction(const FGameplayTag& NewParkourAction);

	bool SetClimbStyle(const FGameplayTag& NewClimbStyle);

	bool SetClimbMovement(const FGameplayTag& NewDirection);

	bool SetLeftHandLedgeLocation(FVector LeftHandLedgeLocation);

	bool SetRightHandLedgeLocation(FVector RightHandLedgeLocation);
	
	bool SetLeftFootLedgeLocation(FVector LeftFootLedgeLocation);
	
	bool SetRightFootLedgeLocation(FVector RightFootLedgeLocation);

	bool SetLeftHandLedgeRotation(FRotator LeftHandLedgeRotation);

	bool SetRightHandLedgeRotation(FRotator RightHandLedgeRotation);
};
