// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ParkourInterface.h"
#include "GameFramework/Character.h"
#include "WidgetActor.h"
#include "ArrowActor.h"
#include "GameplayTagContainer.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ParkourComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PARKOURSYSTEM_API UParkourComponent : public UActorComponent, public IParkourInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UParkourComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* ParkourMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ParkourInputAction;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ParkourAction();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual bool SetInitializeReference(ACharacter* NewCharacter, USpringArmComponent* NewCameraBoom, UMotionWarpingComponent* NewMotionWarping, UCameraComponent* NewCamera) override;

private:

	bool ChekcWallShape();

	ACharacter* Character;
	UCharacterMovementComponent* CharacterMovement;
	USkeletalMeshComponent* CharacterMesh;
	UAnimInstance* AnimInstance;
	UCapsuleComponent* CapsuleComponent;
	USpringArmComponent* CameraBoom;
	UMotionWarpingComponent* MotionWarping;
	UCameraComponent* Camera;
	AWidgetActor* WidgetActor;
	AArrowActor* ArrowActor;
	float ArrowLocationX;
	float ArrowLocationZ;
	float CharacterHeight;
	float FirstTargetArmLenght;
	FVector FirstTargetRelativeLocation;
	FGameplayTag ParkourActionTag;
	bool bAutoClimb;
	bool bCanAutoClimb;
	bool bCanManualClimb;
};
