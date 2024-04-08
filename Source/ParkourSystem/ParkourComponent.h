// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ParkourInterface.h"
#include "GameFramework/Character.h"
#include "WidgetActor.h"
#include "ArrowActor.h"
#include "GameplayTagContainer.h"
#include "ParkourVariables.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ParkourComponent.generated.h"

struct FInputActionValue;

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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ParkourMoveInputAction;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual bool SetInitializeReference(ACharacter* NewCharacter, USpringArmComponent* NewCameraBoom, UMotionWarpingComponent* NewMotionWarping, UCameraComponent* NewCamera) override;

	UFUNCTION()
	void OnParkourMontageBlendOut(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(BlueprintCallable, Category = "ParkourSystem")
	void LeftClimbIK(bool bFirst);
	
	UFUNCTION(BlueprintCallable, Category = "ParkourSystem")
	void RightClimbIK(bool bFirst);
private:
	void ParkourAction();

	void ChekcWallShape();

	void PerformSphereTraceByChannel(UWorld* World, FHitResult& HitResult, const FVector& StartLocation, const FVector& EndLocation, float Radius, ECollisionChannel TraceChannel, bool bDrawDebugSphere) const;

	void PerformLineTraceByChannel(UWorld* World, FHitResult& HitResult, const FVector& StartLocation, const FVector& EndLocation, ECollisionChannel CollisionChannel, bool bDrawDebugLine) const;

	void ShowHitResults();

	void CheckDistance();

	void ParkourType(bool AutoClimb);

	void SetParkourAction(const FGameplayTag& NewParkourAction);

	void SetParkourState(const FGameplayTag& NewParkourState);

	void SetClimbStyle (const FGameplayTag& NewClimbStyle);

	void SetUpParkourSettings(ECollisionEnabled::Type CollsionType, EMovementMode MovementMode, FRotator RotationRate, bool bDoCollisionTest, bool bStopImmediately);

	bool CheckMantleSurface();

	bool CheckVaultSurface();

	bool CheckClimbSurface();

	void CheckClimbStyle();

	void SecondClimbLedgeResultCalculation();



	void PlayParkourMontage();

	FVector FindWarpLocation(const FVector& ImpactPoint, float XOffset, float ZOffset) const;

	FVector FindWarpLocationChecked(const FVector& ImpactPoint, float XOffset, float ZOffset) const;

	void ResetParkourResults();

	void Move(const FInputActionValue& Value);

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
	float CharacterHandUp;
	float CharacterHandFront;
	float FirstTargetArmLenght;
	float ForwardScale;
	float RightScale;
	FVector FirstTargetRelativeLocation;
	FGameplayTag ParkourActionTag;
	FGameplayTag ParkourStateTag;
	FGameplayTag ClimbStyle;
	FGameplayTag BlendOutState;
	bool bAutoClimb;
	bool bCanAutoClimb;
	bool bCanManualClimb;
	bool bShowHitResult;
	bool bDrawDebug;
	bool bOnGround;
	bool bFirstClimbMove;
	float WallHeight;
	float WallDepth;
	float VaultHeight;
	TArray<FHitResult> WallHitTraces;
	TArray<FHitResult> HopHitTraces;
	FHitResult WallHitResult;
	FHitResult WallTopResult;
	FHitResult TopHits;
	FHitResult WallDepthResult;
	FHitResult WallVaultResult;
	FHitResult SecondClimbLedgeResult;
	FRotator WallRotation;
	UParkourVariables* ParkourVariables;
};
