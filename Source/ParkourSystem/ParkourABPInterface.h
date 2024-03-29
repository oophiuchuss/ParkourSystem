// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ParkourABPInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable, MinimalAPI)
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
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ParkourSystem")
	bool SetParkourState(const FGameplayTag& NewParkourState);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ParkourSystem")
	bool SetParkourAction(const FGameplayTag& NewParkourAction);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ParkourSystem")
	bool SetClimbStyle(const FGameplayTag& NewClimbStyle);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ParkourSystem")
	bool SetClimbMovement(const FGameplayTag& NewDirection);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ParkourSystem")
	bool SetLeftHandLedgeLocation(FVector LeftHandLedgeLocation);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ParkourSystem")
	bool SetRightHandLedgeLocation(FVector RightHandLedgeLocation);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ParkourSystem")
	bool SetLeftFootLedgeLocation(FVector LeftFootLedgeLocation);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ParkourSystem")
	bool SetRightFootLedgeLocation(FVector RightFootLedgeLocation);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ParkourSystem")
	bool SetLeftHandLedgeRotation(FRotator LeftHandLedgeRotation);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ParkourSystem")
	bool SetRightHandLedgeRotation(FRotator RightHandLedgeRotation);
};
