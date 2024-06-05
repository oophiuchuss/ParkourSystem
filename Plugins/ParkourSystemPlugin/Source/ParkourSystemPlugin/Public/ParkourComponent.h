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

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PARKOURSYSTEMPLUGIN_API UParkourComponent : public UActorComponent, public IParkourInterface
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

	UFUNCTION()
	void OnCornerMoveCompleted();

	void LeftClimbIK();

	void RightClimbIK();
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

	void CornerMove(const FVector& TargerRelativeLocation, const FRotator& TargerRelativeRotation);

	void OutCornerMove(const int32& OutCornerIndex);

	void CornerHop(bool bIsOutCorner);

	void StopClimbMovement();

	void ResetMovement();
	////////////////////////////////////////////////////////////////////

	//SURFACE CHECK

	void ChekcWallShape();

	bool CheckMantleSurface();

	bool CheckVaultSurface();

	bool CheckClimbSurface();

	void CheckClimbStyle();

	void PerformSphereTraceByChannel(UWorld* World, FHitResult& HitResult, const FVector& StartLocation, const FVector& EndLocation,
		float Radius, ECollisionChannel TraceChannel, bool bDrawDebugSphere, float DrawTime = 2.0f) const;

	void PerformBoxTraceByChannel(UWorld* World, FHitResult& HitResult, const FVector& StartLocation, const FVector& EndLocation,
		const FVector& BoxHalfExtend, ECollisionChannel CollisionChannel, bool bDrawDebugBox, float DrawTime = 2.0f) const;

	void PerformCapsuleTraceByChannel(UWorld* World, FHitResult& HitResult, const FVector& StartLocation, const FVector& EndLocation,
		float HalfHeight, float Radius, ECollisionChannel CollisionChannel, bool bDrawDebugCapsule, float DrawTime = 2.0f) const;

	void PerformLineTraceByChannel(UWorld* World, FHitResult& HitResult, const FVector& StartLocation, const FVector& EndLocation,
		ECollisionChannel CollisionChannel, bool bDrawDebugLine, float DrawTime = 1.0f) const;

	void ShowHitResults();

	void CheckDistance();

	void ResetParkourResults();

	float FirstTraceHeight() const;

	void CheckClimbOrHop();

	bool CheckAirHang() const;

	bool CheckClimbMoveSurface(const FHitResult& MovementHitResult) const;

	bool CheckOutCorner(int32& OutCornerIndex) const;

	bool CheckInCorner();

	bool CheckCornerHop();
	
	bool OutCornerHop();

	bool CheckLedgeValid();

	//TODO maybe in other category
	FRotator GetDesireRotation() const;

	void FindDropDownHangLocation();

	void FindHopLocation();

	void GetClimbForwardValue(float ScaleValue, float& HorizontalForwardValue, float& VerticalForwardValue) const;

	void GetClimbRightValue(float ScaleValue, float& HorizontalRightValue, float& VerticalRightValue) const;

	float GetVerticalAxis() const;

	float GetHorizontalAxis() const;

	FGameplayTag GetClimbDesireRotation();

	float GetClimbMoveSpeed() const;

	float GetClimbLeftHandZOffset() const;

	float GetClimbRightHandZOffset() const;

	FGameplayTag SelectHopAction();

	FGameplayTag GetHopDirection() const;

	////////////////////////////////////////////////////////////////////

	//SET UP STATES AND ACTIONS

	void ParkourType(bool bAutoClimb);

	void SetParkourAction(const FGameplayTag& NewParkourAction);

	void SetParkourState(const FGameplayTag& NewParkourState);

	void SetClimbStyle(const FGameplayTag& NewClimbStyle);

	void SetClimbDirection(const FGameplayTag& NewClimbDirection);

	void SetUpParkourSettings(ECollisionEnabled::Type CollsionType, EMovementMode MovementMode, FRotator RotationRate, bool bDoCollisionTest, bool bStopImmediately);

	void SetClimbStyleOnMove(const FHitResult& HitResult, const FRotator& Rotation);


	////////////////////////////////////////////////////////////////////

	//DYNAMIC CAMERA
	void PreviousStateSettings(const FGameplayTag& PreviousState, const FGameplayTag& NewState);

	void AddCameraTimeline(float Time);

	void CameraTimelineTick();

	void FinishTimeline();


	////////////////////////////////////////////////////////////////////

	//INVERSE KINEMATICS
	void FirstClimbLedgeResultCalculation();
	
	void SecondClimbLedgeResultCalculation();

	void LeftHandLedgeIK(FHitResult& LedgeResult);

	void LeftFootIK(FHitResult& LedgeResult);

	void RightHandLedgeIK(FHitResult& LedgeResult);

	void RightFootIK(FHitResult& LedgeResult);

	void ClimbMoveIK();

	void ClimbMoveHandIK();

	void ClimbMoveLeftHandIK();

	void ClimbMoveRightHandIK();

	void ClimbMoveFootIK();

	void ClimbMoveLeftFootIK();

	void ClimbMoveRightFootIK();

	void ResetFootIK(bool bIsLeft);

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
	float CharacterHeightDiff;
	float CharacterHandUp;
	float CharacterHandFront;
	float FirstCameraTargetArmLenght;
	float TargetArmLenght;
	float ForwardScale;
	float RightScale;
	FVector FirstTargetRelativeLocation;
	FVector TargetRelativeCameraLocation;
	FVector LastClimbRightHandLocation;
	FVector LastClimbLeftHandLocation;
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
	bool bCanCornerHop;
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
	float ClimbHandSpace;
	float VerticalHopDistance;
	float HorizontalHopDistance;
	TArray<FHitResult> WallHitTraces;
	TArray<FHitResult> HopHitTraces;
	FHitResult WallHitResult;
	FHitResult WallTopResult;
	FHitResult TopHits;
	FHitResult WallDepthResult;
	FHitResult WallVaultResult;
	FHitResult FirstClimbLedgeResult;
	FHitResult SecondClimbLedgeResult;
	FHitResult LeftHandClimbResult;
	FHitResult RightHandClimbResult;
	FRotator WallRotation;
	FRotator CornerHopRotation;
	UParkourVariables* ParkourVariables;
	FTimerHandle TimerHandle_DelayedFunction;
	FTimerHandle TimerHandle_FinishCameraTimeline;
	FTimerHandle TimerHandle_TickCameraTimeline;
	UCurveFloat* CameraCurve;
};
