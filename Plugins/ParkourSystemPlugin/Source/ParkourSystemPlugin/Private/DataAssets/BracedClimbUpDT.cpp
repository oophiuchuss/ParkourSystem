// Fill out your copyright notice in the Description page of Project Settings.


#include "BracedClimbUpDT.h"

UBracedClimbUpDT::UBracedClimbUpDT()
{
	FString path = "/ParkourSystemPlugin/Animations/ParkourAnimations/Montages/BracedClimbUpTheLedge_Montage";
	ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(*path);

	if (MontageAsset.Succeeded())
		ParkourMontage = MontageAsset.Object;
	else
		UE_LOG(LogTemp, Warning, TEXT("UBracedClimbUpDT: AnimMontage wasn't found"));

	ParkourInState = FGameplayTag::RequestGameplayTag("Parkour.State.Mantle");
	ParkourOutState = FGameplayTag::RequestGameplayTag("Parkour.State.NotBusy");

	Warp1XOffset = 10.0f;
	Warp1ZOffset = 3.0f;
	Warp2XOffset = 0.0f;
	Warp2ZOffset = 0.0f;
	Warp3XOffset = 0.0f;
	Warp3ZOffset = 0.0f;
	MontageStartPosition = 0.2f;
	FallingMontageStartPosition = 0.0f;
}
