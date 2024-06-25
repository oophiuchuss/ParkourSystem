// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ParkourFunctionLibrary.generated.h"

/**
 * Utility functions for parkour actions within the Parkour System.
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API UParkourFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Calculates a rotation where the Z axis is reversed relative to the given normal vector.
	UFUNCTION(BlueprintCallable, Category = "ParkourSystem")
	static FRotator NormalReverseRotationZ(FVector Normal);

	// Reverses the given rotator`s Yaw by 180 degrees.
	UFUNCTION(BlueprintCallable, Category = "ParkourSystem")
	static void ReverseRotation(FRotator& Rotator);

	// Selects a float value based on the climb style.
	UFUNCTION(BlueprintCallable, Category = "ParkourSystem")
	static float SelectClimbStyleFloat(float Braced, float FreeHang, const FGameplayTag& ClimbStyle);

	// Selects a float value based on the direction of movement.
	UFUNCTION(BlueprintCallable, Category = "ParkourSystem")
	static float SelectDirectionFloat(float Forward, float Backward, float Left, float Right,
		float ForwardLeft, float BackwardLeft, float ForwardRight, float BackwardRight, const FGameplayTag& Direction);

	// Selects a float value based on the parkour state.
	UFUNCTION(BlueprintCallable, Category = "ParkourSystem")
	static float SelectParkourStateFloat(float NotBusy, float Vault, float Mantle, float Climb, const FGameplayTag& ParkourState);

	// Selects a gameplay tag for hop action based on the direction of movement.
	UFUNCTION(BlueprintCallable, Category = "ParkourSystem")
	static const FGameplayTag& SelectDirectionHopAction(const FGameplayTag& Forward, const FGameplayTag& Backward, const FGameplayTag& Left, const FGameplayTag& Right,
		const FGameplayTag& ForwardLeft, const FGameplayTag& BackwardLeft, const FGameplayTag& ForwardRight, const FGameplayTag& BackwardRight, const FGameplayTag& Direction);
};
