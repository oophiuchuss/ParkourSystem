// Fill out your copyright notice in the Description page of Project Settings.


#include "VaultDT.h"

UVaultDT::UVaultDT()
{
	FString path = "/ParkourSystem/Animations/ParkourAnimations/Mantle/VaultUE5_Montage";
	ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(*path);
	if (MontageAsset.Succeeded())
	{
		ParkourMontage = MontageAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayParkourMontage: AnimMontage wasn't found"));
	}

	ParkourInState = FGameplayTag::RequestGameplayTag("Parkour.State.Vault");
	ParkourOutState = FGameplayTag::RequestGameplayTag("Parkour.State.NotBusy");

	Warp1XOffset = -70.0f;
	Warp1ZOffset = -60.0;
	Warp2XOffset = -30.0f;
	Warp2ZOffset = -45.0f;
	Warp3XOffset = 0.0f;
	Warp3ZOffset = 3.0f;
	MontageStartPosition = 0.2f;
	
}
