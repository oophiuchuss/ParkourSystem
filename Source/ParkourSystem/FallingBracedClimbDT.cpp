// Fill out your copyright notice in the Description page of Project Settings.


#include "FallingBracedClimbDT.h"

UFallingBracedClimbDT::UFallingBracedClimbDT()
{
	FString path = "/ParkourSystem/Animations/ParkourAnimations/Montages/FallingBracedClimb_Montage";
	ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(*path);

	if (MontageAsset.Succeeded())
		ParkourMontage = MontageAsset.Object;
	else
		UE_LOG(LogTemp, Warning, TEXT("UFallingBracedClimbDT: AnimMontage wasn't found"));

	ParkourInState = FGameplayTag::RequestGameplayTag("Parkour.State.ReachLedge");
	ParkourOutState = FGameplayTag::RequestGameplayTag("Parkour.State.Climb");

	Warp1XOffset = -43;
	Warp1ZOffset = -105.0f;
	Warp2XOffset = -50.0f;
	Warp2ZOffset = -200.0f;
	Warp3XOffset = 0.0f;
	Warp3ZOffset = 0.0f;
	MontageStartPosition = 0.2f;
	FallingMontageStartPosition = 0.4f;
}
