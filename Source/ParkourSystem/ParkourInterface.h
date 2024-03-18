// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "MotionWarpingComponent.h"

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ParkourInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UParkourInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PARKOURSYSTEM_API IParkourInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	bool SetInitializeRefference(const ACharacter& Character, const USpringArmComponent& CameraBoom, const UMotionWarpingComponent& MotionWarping, const UCameraComponent& Camera);
};
