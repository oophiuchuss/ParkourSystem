// Fill out your copyright notice in the Description page of Project Settings.


#include "FreeHangClimbUpDT.h"

UFreeHangClimbUpDT::UFreeHangClimbUpDT()
{
	FString path = "/ParkourSystem/Animations/ParkourAnimations/Montages/FreeHangClimbUpUE5_Montage";
	ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(*path);

	if (MontageAsset.Succeeded())
		ParkourMontage = MontageAsset.Object;
	else
		UE_LOG(LogTemp, Warning, TEXT("UFreeHangClimbUpDT: AnimMontage wasn't found"));

	ParkourInState = FGameplayTag::RequestGameplayTag("Parkour.State.Mantle");
	ParkourOutState = FGameplayTag::RequestGameplayTag("Parkour.State.NotBusy");

	Warp1XOffset = -5.0f;
	Warp1ZOffset = -118.0f;
	Warp2XOffset = -30.0f;
	Warp2ZOffset = 1.5f;
	Warp3XOffset = 0.0f;
	Warp3ZOffset = 0.0f;
	MontageStartPosition = 1.0f;
	FallingMontageStartPosition = 0.0f;
}
