// Fill out your copyright notice in the Description page of Project Settings.


#include "FallingFreeHangClimb.h"

UFallingFreeHangClimb::UFallingFreeHangClimb()
{
	FString path = "/ParkourSystem/Animations/ParkourAnimations/Montages/FallingFreeHangClimb_Montage";
	ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(*path);

	if (MontageAsset.Succeeded())
		ParkourMontage = MontageAsset.Object;
	else
		UE_LOG(LogTemp, Warning, TEXT("UBracedClimbUpDT: AnimMontage wasn't found"));

	ParkourInState = FGameplayTag::RequestGameplayTag("Parkour.State.ReachLedge");
	ParkourOutState = FGameplayTag::RequestGameplayTag("Parkour.State.Climb");

	Warp1XOffset = -20.0f;
	Warp1ZOffset = -140.0f;
	Warp2XOffset = -8.0f;
	Warp2ZOffset = -200.0f;
	Warp3XOffset = 0.0f;
	Warp3ZOffset = 0.0f;
	MontageStartPosition = 0.0f;
	FallingMontageStartPosition = 6.0f;
}
