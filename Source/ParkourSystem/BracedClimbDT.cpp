// Fill out your copyright notice in the Description page of Project Settings.


#include "BracedClimbDT.h"

UBracedClimbDT::UBracedClimbDT()
{
	FString path = "/ParkourSystem/Animations/ParkourAnimations/Montages/BracedClimbUE5_Montage";
	ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(*path);
	
	if (MontageAsset.Succeeded())
		ParkourMontage = MontageAsset.Object;
	else
		UE_LOG(LogTemp, Warning, TEXT("UBracedClimbDT: AnimMontage wasn't found"));

	ParkourInState = FGameplayTag::RequestGameplayTag("Parkour.State.ReachLedge");
	ParkourOutState = FGameplayTag::RequestGameplayTag("Parkour.State.Climb");

	Warp1XOffset = -50.0f;
	Warp1ZOffset = -180.0;
	Warp2XOffset = 0.0f;
	Warp2ZOffset = 0.0f;
	Warp3XOffset = 0.0f;
	Warp3ZOffset = 0.0f;
	MontageStartPosition = 0.2f;
	FallingMontageStartPosition = 0.4f;
}
