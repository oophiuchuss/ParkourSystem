#pragma once

#include "ParkourVariables.h"
#include "CoreMinimal.h"
#include "ParkourVariablesCollection.generated.h"

/**
 * FParkourVariablesCollection is struct that contains all needed Data Assests for parkour component.
 * This struct allows easy access to all DTs and gives an ability to easily modify data
 */
USTRUCT(BlueprintType)
struct FParkourVariablesCollection
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourActionDataAssets")
	TObjectPtr<UParkourVariables> BracedClimbDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourActionDataAssets")
	TObjectPtr<UParkourVariables> BracedClimbUpDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourActionDataAssets")
	TObjectPtr<UParkourVariables> BracedDropDownDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourActionDataAssets")
	TObjectPtr<UParkourVariables> ClimbHopDownDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourActionDataAssets")
	TObjectPtr<UParkourVariables> ClimbHopLeftDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourActionDataAssets")
	TObjectPtr<UParkourVariables> ClimbHopLeftUpDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourActionDataAssets")
	TObjectPtr<UParkourVariables> ClimbHopRightDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourActionDataAssets")
	TObjectPtr<UParkourVariables> ClimbHopRightUpDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourActionDataAssets")
	TObjectPtr<UParkourVariables> ClimbHopUpDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourActionDataAssets")
	TObjectPtr<UParkourVariables> FallingBracedClimbDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourActionDataAssets")
	TObjectPtr<UParkourVariables> FallingFreeHangClimbDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourActionDataAssets")
	TObjectPtr<UParkourVariables> FreeClimbHopDownDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourActionDataAssets")
	TObjectPtr<UParkourVariables> FreeClimbHopLeftDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourActionDataAssets")
	TObjectPtr<UParkourVariables> FreeClimbHopRightDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourActionDataAssets")
	TObjectPtr<UParkourVariables> FreeHangClimbUpDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourActionDataAssets")
	TObjectPtr<UParkourVariables> FreeHangDropDownDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourActionDataAssets")
	TObjectPtr<UParkourVariables> FreeHangDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourActionDataAssets")
	TObjectPtr<UParkourVariables> HighVaultDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourActionDataAssets")
	TObjectPtr<UParkourVariables> LowMantleDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourActionDataAssets")
	TObjectPtr<UParkourVariables> MantleDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourActionDataAssets")
	TObjectPtr<UParkourVariables> ThinVaultDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourActionDataAssets")
	TObjectPtr<UParkourVariables> VaultDT;
};