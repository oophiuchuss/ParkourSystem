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
 * Interface for handling parkour-related states and actions in animation blueprints.
 */
class PARKOURSYSTEMPLUGIN_API IParkourABPInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// Sets the current parkour state.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ParkourSystem")
	bool SetParkourState(const FGameplayTag& NewParkourState);

	// Sets the current parkour action.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ParkourSystem")
	bool SetParkourAction(const FGameplayTag& NewParkourAction);

	// Sets the current climb style.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ParkourSystem")
	bool SetClimbStyle(const FGameplayTag& NewClimbStyle);

	// Sets the current direction for climb movement.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ParkourSystem")
	bool SetClimbMovement(const FGameplayTag& NewDirection);

	// Sets the location of the left hand on the ledge.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ParkourSystem")
	bool SetLeftHandLedgeLocation(FVector LeftHandLedgeLocation);

	// Sets the location of the right hand on the ledge.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ParkourSystem")
	bool SetRightHandLedgeLocation(FVector RightHandLedgeLocation);

	// Sets the location of the left foot.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ParkourSystem")
	bool SetLeftFootLocation(FVector LeftFootLocation);

	// Sets the location of the right foot.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ParkourSystem")
	bool SetRightFootLocation(FVector RightFootLocation);

	// Sets the rotation of the left hand on the ledge.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ParkourSystem")
	bool SetLeftHandLedgeRotation(FRotator LeftHandLedgeRotation);

	// Sets the rotation of the right hand on the ledge.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ParkourSystem")
	bool SetRightHandLedgeRotation(FRotator RightHandLedgeRotation);
};
