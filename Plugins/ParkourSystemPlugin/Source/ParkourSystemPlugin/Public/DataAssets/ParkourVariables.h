// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Animation/AnimMontage.h"
#include "GameplayTagContainer.h"

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ParkourVariables.generated.h"

/**
 * 
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API UParkourVariables : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParkourSystem")
	UAnimMontage* ParkourMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParkourSystem")
	FGameplayTag  ParkourInState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParkourSystem")
	FGameplayTag  ParkourOutState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParkourSystem")
	float Warp1XOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParkourSystem")
	float Warp1ZOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParkourSystem")
	float Warp2XOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParkourSystem")
	float Warp2ZOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParkourSystem")
	float Warp3XOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParkourSystem")
	float Warp3ZOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParkourSystem")
	float MontageStartPosition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParkourSystem")
	float FallingMontageStartPosition;
};
