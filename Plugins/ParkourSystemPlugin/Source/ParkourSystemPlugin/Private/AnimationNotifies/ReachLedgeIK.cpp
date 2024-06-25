// Fill out your copyright notice in the Description page of Project Settings.


#include "ReachLedgeIK.h"
#include "ParkourComponent.h"
#include "Components/SkeletalMeshComponent.h"

void UReachLedgeIK::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	UParkourComponent* ParkourComponent = MeshComp->GetOwner()->GetComponentByClass<UParkourComponent>();
	
	if (!ParkourComponent)
		return;

	//Activate left or right IK
	if (bIsLeft)
		ParkourComponent->LeftClimbIK();
	else
		ParkourComponent->RightClimbIK();
}
