// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ParkourFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API UParkourFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "ParkourSystem")
		static FRotator NormalReverseRotationZ(FVector Normal);

	UFUNCTION(BlueprintCallable, Category = "ParkourSystem")
		static void ReverseRotation(FRotator& Rotator);
	
	UFUNCTION(BlueprintCallable, Category = "ParkourSystem")
		static float SelectClimbStyleFloat(float Braced, float FreeHang, const FGameplayTag& ClimbStyle);

	UFUNCTION(BlueprintCallable, Category = "ParkourSystem")
		static float SelectDirectionFloat(float Forward, float Backward, float Left, float Right,
			float ForwardLeft, float BackwardLeft, float ForwardRight, float BackwardRight, const FGameplayTag& Direction);

	UFUNCTION(BlueprintCallable, Category = "ParkourSystem")
		static float SelectParkourStateFloat(float NotBusy, float Vault, float Mantle, float Climb, const FGameplayTag& ParkourState);

	UFUNCTION(BlueprintCallable, Category = "ParkourSystem")
		static const FGameplayTag& SelectDirectionHopAction(const FGameplayTag& Forward, const FGameplayTag& Backward, const FGameplayTag& Left, const FGameplayTag& Right,
			const FGameplayTag& ForwardLeft, const FGameplayTag& BackwardLeft, const FGameplayTag& ForwardRight, const FGameplayTag& BackwardRight, const FGameplayTag& Direction);
};
