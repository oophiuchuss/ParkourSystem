// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ReachLedgeIK.generated.h"

/**
 * UReachLedgeIK is a custom animation notify state for ledge reaching in the Parkour System.
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API UReachLedgeIK : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	// Whether the animation represents a left-side ledge reach or right.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem")
	bool bIsLeft;

	// Called when the animation notify state ends.
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
