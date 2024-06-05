// Fill out your copyright notice in the Description page of Project Settings.


#include "FreeHangDT.h"

UFreeHangDT::UFreeHangDT()
{
	FString path = "/ParkourSystemPlugin/Animations/ParkourAnimations/Montages/FreeHangClimb_Montage";
	ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(*path);
	
	if (MontageAsset.Succeeded())
		ParkourMontage = MontageAsset.Object;
	else
		UE_LOG(LogTemp, Warning, TEXT("UFreeHangDT: AnimMontage wasn't found"));

	ParkourInState = FGameplayTag::RequestGameplayTag("Parkour.State.ReachLedge");
	ParkourOutState = FGameplayTag::RequestGameplayTag("Parkour.State.Climb");

	Warp1XOffset = -50.0f;
	Warp1ZOffset = -203.0f;
	Warp2XOffset = 7.0f;
	Warp2ZOffset = -203.0f;
	Warp3XOffset = 0.0f;
	Warp3ZOffset = 0.0f;
	MontageStartPosition = 0.0f;
	FallingMontageStartPosition = 0.6f;
}
