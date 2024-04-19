// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ParkourInterface.h"
#include "GameFramework/Character.h"
#include "WidgetActor.h"
#include "ArrowActor.h"
#include "GameplayTagContainer.h"
#include "ParkourVariables.h"
#include "Curves/CurveFloat.h"

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
	class UInputAction* ParkourDropInputAction;	
	
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

	////////////////////////////////////////////////////////////////////

	//PARKOUR ACTION

	void ParkourAction();

	void ParkourActionFunction(bool bAutoClimb);

	void AutoClimb();

	void ParkourDrop();

	void SetCanManualClimb();

	void Move(const FInputActionValue& Value);

	void ClimbMove();
	
	void StopClimbMovement();

	void ResetMovement();
	////////////////////////////////////////////////////////////////////

	//SURFACE CHECK

	void ChekcWallShape();

	bool CheckMantleSurface();

	bool CheckVaultSurface();

	bool CheckClimbSurface();

	void CheckClimbStyle();

	void PerformSphereTraceByChannel(UWorld* World, FHitResult& HitResult, const FVector& StartLocation, const FVector& EndLocation, float Radius, ECollisionChannel TraceChannel, bool bDrawDebugSphere) const;

	void PerformLineTraceByChannel(UWorld* World, FHitResult& HitResult, const FVector& StartLocation, const FVector& EndLocation, ECollisionChannel CollisionChannel, bool bDrawDebugLine) const;

	void ShowHitResults();

	void CheckDistance();

	void ResetParkourResults();

	float FirstTraceHeight() const;

	void CheckClimbOrHop();

	bool CheckAirHang() const;

	//TODO maybe in other category
	FRotator GetDesireRotation() const;

	void FindDropDownHangLocation();

	void GetClimbForwardValue(float ScaleValue, float& HorizontalForwardValue, float& VerticalForwardValue) const;

	void GetClimbRightValue(float ScaleValue, float& HorizontalRightValue, float& VerticalRightValue) const;

	float GetVerticalAxis() const;

	float GetHorizontalAxis() const;

	FGameplayTag GetClimbDesireRotation();

	////////////////////////////////////////////////////////////////////

	//SET UP STATES AND ACTIONS

	void ParkourType(bool bAutoClimb);

	void SetParkourAction(const FGameplayTag& NewParkourAction);

	void SetParkourState(const FGameplayTag& NewParkourState);

	void SetClimbStyle (const FGameplayTag& NewClimbStyle);

	void SetClimbDirection (const FGameplayTag& NewClimbDirection);
	
	void SetUpParkourSettings(ECollisionEnabled::Type CollsionType, EMovementMode MovementMode, FRotator RotationRate, bool bDoCollisionTest, bool bStopImmediately);

	////////////////////////////////////////////////////////////////////

	//DYNAMIC CAMERA
	void PreviousStateSettings(const FGameplayTag& PreviousState, const FGameplayTag& NewState);

	void AddCameraTimeline(float Time);

	void CameraTimelineTick();
	
	void FinishTimeline();


	////////////////////////////////////////////////////////////////////

	//INVERSE KINEMATICS
	void SecondClimbLedgeResultCalculation();

	void LeftHandLedgeIK(FHitResult& LedgeResult);
	
	void LeftFootIK(FHitResult& LedgeResult);
	
	void RightHandLedgeIK(FHitResult& LedgeResult);
	
	void RightFootIK(FHitResult& LedgeResult);


	////////////////////////////////////////////////////////////////////

	//MONTAGES
	void PlayParkourMontage();

	FVector FindWarpLocation(const FVector& ImpactPoint, float XOffset, float ZOffset) const;

	FVector FindWarpLocationChecked(const FVector& ImpactPoint, float XOffset, float ZOffset) const;

	void FindMontageStartTime();

	////////////////////////////////////////////////////////////////////

	//VARIABLES
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
	float FirstCameraTargetArmLenght;
	float TargetArmLenght;
	float ForwardScale;
	float RightScale;
	FVector FirstTargetRelativeLocation;
	FVector TargetRelativeCameraLocation;
	FGameplayTag ParkourActionTag;
	FGameplayTag ParkourStateTag;
	FGameplayTag ClimbStyle;
	FGameplayTag ClimbDirection;
	FGameplayTag BlendOutState;
	bool bCanAutoClimb;
	bool bCanManualClimb;
	bool bShowHitResult;
	bool bDrawDebug;
	bool bOnGround;
	bool bFirstClimbMove;
	float WallHeight;
	float WallDepth;
	float VaultHeight;
	float CameraCurveAlpha;
	float MontageStartTime;
	float HorizontalClimbForwardValue;
	float VerticalClimbForwardValue;
	float HorizontalClimbRightValue;
	float VerticalClimbRightValue;
	float DesireRotationZ;
	float DesireRotationY;
	float ClimbMoveCheckDistance;
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
	FTimerHandle TimerHandle_DelayedFunction;
	FTimerHandle TimerHandle_FinishCameraTimeline;
	FTimerHandle TimerHandle_TickCameraTimeline;
	UCurveFloat* CameraCurve;
};
