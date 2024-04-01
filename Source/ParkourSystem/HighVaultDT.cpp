// Fill out your copyright notice in the Description page of Project Settings.


#include "HighVaultDT.h"

UHighVaultDT::UHighVaultDT()
{
	FString path = "/ParkourSystem/Animations/ParkourAnimations/Mantle/HighVaultUE5_Montage";
	ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(*path);
	if (MontageAsset.Succeeded())
	{
		ParkourMontage = MontageAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UThinVaultDT: AnimMontage wasn't found"));
	}

	ParkourInState = FGameplayTag::RequestGameplayTag("Parkour.State.Vault");
	ParkourOutState = FGameplayTag::RequestGameplayTag("Parkour.State.NotBusy");

	Warp1XOffset = -90.0f;
	Warp1ZOffset = -90.0;
	Warp2XOffset = -50.0f;
	Warp2ZOffset = -30.0f;
	Warp3XOffset = 0.0f;
	Warp3ZOffset = 3.0f;
	MontageStartPosition = 0.2f;
}