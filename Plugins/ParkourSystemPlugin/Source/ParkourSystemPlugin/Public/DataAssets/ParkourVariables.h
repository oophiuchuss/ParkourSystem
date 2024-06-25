// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Animation/AnimMontage.h"
#include "GameplayTagContainer.h"

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ParkourVariables.generated.h"

/**
 * UParkourVariables is Data asset model containing variables related to parkour gameplay.
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API UParkourVariables : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// Animation montage for parkour actions.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParkourSystem")
	UAnimMontage* ParkourMontage;

	// Gameplay tag representing the state when entering parkour.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParkourSystem")
	FGameplayTag ParkourInState;

	// Gameplay tag representing the state when exiting parkour.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParkourSystem")
	FGameplayTag ParkourOutState;

	// X offset for the first warp position during parkour.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParkourSystem")
	float Warp1XOffset;

	// Z offset for the first warp position during parkour.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParkourSystem")
	float Warp1ZOffset;

	// X offset for the second warp position during parkour.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParkourSystem")
	float Warp2XOffset;

	// Z offset for the second warp position during parkour.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParkourSystem")
	float Warp2ZOffset;

	// X offset for the third warp position during parkour.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParkourSystem")
	float Warp3XOffset;

	// Z offset for the third warp position during parkour.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParkourSystem")
	float Warp3ZOffset;

	// Starting position within the parkour montage.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParkourSystem")
	float MontageStartPosition;

	// Starting position within the falling montage during parkour.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParkourSystem")
	float FallingMontageStartPosition;
};
