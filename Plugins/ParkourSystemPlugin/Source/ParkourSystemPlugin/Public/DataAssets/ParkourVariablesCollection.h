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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParkourVariables> BracedClimbDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParkourVariables> BracedClimbUpDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParkourVariables> BracedDropDownDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParkourVariables> ClimbHopDownDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParkourVariables> ClimbHopLeftDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParkourVariables> ClimbHopLeftUpDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParkourVariables> ClimbHopRightDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParkourVariables> ClimbHopRightUpDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParkourVariables> ClimbHopUpDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParkourVariables> FallingBracedClimbDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParkourVariables> FallingFreeHangClimbDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParkourVariables> FreeClimbHopDownDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParkourVariables> FreeClimbHopLeftDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParkourVariables> FreeClimbHopRightDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParkourVariables> FreeHangClimbUpDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParkourVariables> FreeHangDropDownDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParkourVariables> FreeHangDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParkourVariables> HighVaultDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParkourVariables> LowMantleDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParkourVariables> MantleDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParkourVariables> ThinVaultDT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParkourVariables> VaultDT;
};