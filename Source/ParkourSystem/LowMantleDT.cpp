// Fill out your copyright notice in the Description page of Project Settings.


#include "LowMantleDT.h"

ULowMantleDT::ULowMantleDT()
{
	FString path = "/ParkourSystem/Animations/ParkourAnimations/Montages/LowMantle_Montage";
	ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(*path);
	
	if (MontageAsset.Succeeded())
		ParkourMontage = MontageAsset.Object;
	else
		UE_LOG(LogTemp, Warning, TEXT("ULowMantleDT: AnimMontage wasn't found"));

	ParkourInState = FGameplayTag::RequestGameplayTag("Parkour.State.Mantle");
	ParkourOutState = FGameplayTag::RequestGameplayTag("Parkour.State.NotBusy");

	Warp1XOffset = -20.0f;
	Warp1ZOffset = -20.0;
	Warp2XOffset = 15.0f;
	Warp2ZOffset = 2.0f;
	Warp3XOffset = 0.0f;
	Warp3ZOffset = 0.0f;
	MontageStartPosition = 0.2f;
}
