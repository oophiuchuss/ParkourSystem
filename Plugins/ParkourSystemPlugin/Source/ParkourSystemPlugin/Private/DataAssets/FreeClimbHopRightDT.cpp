// Fill out your copyright notice in the Description page of Project Settings.


#include "FreeClimbHopRightDT.h"

UFreeClimbHopRightDT::UFreeClimbHopRightDT()
{
	FString path = "/ParkourSystemPlugin/Animations/ParkourAnimations/Montages/FreeClimbHopRight_Montage";
	ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(*path);

	if (MontageAsset.Succeeded())
		ParkourMontage = MontageAsset.Object;
	else
		UE_LOG(LogTemp, Warning, TEXT("UFreeClimbHopRightDT: AnimMontage wasn't found"));

	ParkourInState = FGameplayTag::RequestGameplayTag("Parkour.State.ReachLedge");
	ParkourOutState = FGameplayTag::RequestGameplayTag("Parkour.State.Climb");

	Warp1XOffset = -10.0f;
	Warp1ZOffset = -200.0f;
	Warp2XOffset = 0.0f;
	Warp2ZOffset = 0.0f;
	Warp3XOffset = 0.0f;
	Warp3ZOffset = 0.0f;
	MontageStartPosition = 0.0f;
	FallingMontageStartPosition = 0.0f;
}
