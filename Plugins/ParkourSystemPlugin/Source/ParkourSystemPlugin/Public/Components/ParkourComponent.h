// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ParkourInterface.h"
#include "GameFramework/Character.h"
#include "WidgetActor.h"
#include "ArrowActor.h"
#include "GameplayTagContainer.h"
#include "Curves/CurveFloat.h"
#include "ParkourVariablesCollection.h"
#include "ParkourParameters.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ParkourComponent.generated.h"

struct FInputActionValue;
struct FParkourVariablesCollection;
class UParkourVariables;
class UInputMappingContext;
class UInputAction;

/**
 * Component for handling parkour-related actions, animations, and state management.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PARKOURSYSTEMPLUGIN_API UParkourComponent : public UActorComponent, public IParkourInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UParkourComponent();

	// Input mapping context for parkour actions.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> ParkourMappingContext;

	// Input action for initiating parkour actions.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ParkourInputAction;

	// Input action for performing parkour drop down.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ParkourDropInputAction;

	// Input action for parkour movement.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ParkourMoveInputAction;

	// Parameters for configuration

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|DataAssets")
	FParkourVariablesCollection ParkourVariablesCollection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FGeneralParams GeneralParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FClimbMoveParams ClimbMoveParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FCornerMoveParams CornerMoveParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FOutCornerMoveParams OutCornerMoveParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FCornerHopParams CornerHopParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FCheckWallShapeParams CheckWallShapeParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FCheckMantleSurfaceParams CheckMantleSurfaceParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FCheckVaultSurfaceParams CheckVaultSurfaceParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FCheckClimbSurfaceParams CheckClimbSurfaceParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FCheckClimbStyleParams CheckClimbStyleParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FFirstTraceHeightParams FirstTraceHeightParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FCheckAirHangParams CheckAirHangParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FCheckClimbMoveSurfaceParams CheckClimbMoveSurfaceParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FCheckOutCornerParams CheckOutCornerParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FCheckInCornerParams CheckInCornerParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FCheckInCornerHopParams CheckInCornerHopParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FCheckOutCornerHopParams CheckOutCornerHopParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FFindDropDownHangLocationParams FindDropDownHangLocationParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FFindHopLocationParams FindHopLocationParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FClimbHandZOffsetParams ClimbHandZOffsetParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FParkourTypeParams ParkourTypeParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FHopDirectionParams HopDirectionParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FPreviousStateSettingsParams PreviousStateSettingsParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FClimbLedgeResultCalculationParams ClimbLedgeResultCalculationParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FHandLedgeIKParams HandLedgeIKParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FSetHandIKParams SetHandIKParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FSetFootIKParams SetFootIKParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FUpdateClimbMoveHandIKParams UpdateClimbMoveHandIKParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FUpdateClimbMoveFootIKParams UpdateClimbMoveFootIKParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem|Parameters")
	FFindWarpLocationCheckedParams FindWarpLocationCheckedParams;


	// Flag to enable or disable debug drawing.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSystem")
	bool bDrawDebug;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Initializes references for character, camera, and motion warping components.
	virtual bool SetInitializeReference(ACharacter* NewCharacter, USpringArmComponent* NewCameraBoom,
		UMotionWarpingComponent* NewMotionWarping, UCameraComponent* NewCamera) override;

	// Handler for parkour montage blend-out event.
	UFUNCTION()
	void OnParkourMontageBlendOut(UAnimMontage* Montage, bool bInterrupted);

	// Handler for completing corner movement.
	UFUNCTION()
	void OnCornerMoveCompleted();

	// Toggles the debug mode for parkour.
	UFUNCTION(BlueprintCallable, Category = "ParkourSystem")
	void ChangeDebugMode();

	// Calculates IK for the left hand and foot.
	void LeftClimbIK();

	// Calculates IK for the right hand and foot.
	void RightClimbIK();

private:

	////////////////////////////////////////////////////////////////////
	//
	//	PARKOUR ACTION HANDLING
	//
	////////////////////////////////////////////////////////////////////

	// Executes the parkour action.
	void ParkourAction();

	// Helper function for executing parkour action depending on auto-climb.
	void ParkourActionFunction(bool bAutoClimb);

	// Executes auto-climb with check whether character is on ground or not.
	void AutoClimb();

	// Performs parkour drop action.
	void ParkourDrop();

	// Enables manual climb mode. Used for delayed disabling.
	void SetCanManualClimb();

	// Handles parkour movement input.
	void Move(const FInputActionValue& Value);

	// Executes climb movement.
	void ClimbMove();

	// Executes hop action.
	void HopAction();

	// Handles movement around a corner with provided final location and rotation. 
	void CornerMove(const FVector& TargerRelativeLocation, const FRotator& TargerRelativeRotation);

	// Handles movement out of a corner with provided index of check iteration.
	void OutCornerMove(const int32& OutCornerIndex);

	// Handles hop action for corner transitions.
	void CornerHop(bool bIsOutCorner);

	// Stops climb movement.
	void StopClimbMovement();

	// Resets movement to default state.
	void ResetMovement();

	////////////////////////////////////////////////////////////////////
	//
	//	SURFACE CHECKS AND VALIDATION
	//
	////////////////////////////////////////////////////////////////////

	// Checks the shape of the wall for climbing.
	void ChekcWallShape();

	// Checks if a surface is suitable for mantling.
	bool CheckMantleSurface();

	// Checks if a surface is suitable for vaulting.
	bool CheckVaultSurface();

	// Checks if a surface is suitable for climbing.
	bool CheckClimbSurface();

	// Determines the climb style based on the surface. 
	void CheckClimbStyle(const FHitResult& HitResult, const FRotator& Rotation);

	// Returns the height for the first trace for checking wall. 
	float FirstTraceHeight() const;

	// Checks and executes air hang or climb up 
	void CheckAirHangOrClimb();

	// Determines if the action should be a climb or hop.
	void CheckClimbOrHop();

	// Checks if the character can hang in the air.
	bool CheckAirHang() const;

	// Validates the distance like WallHeight, WallDepth and VaultHeight.
	void CheckDistance();

	// Validates the surface for climb movement.
	bool CheckClimbMoveSurface(const FHitResult& MovementHitResult) const;

	// Checks for valid out-corner conditions with provided index of check iteration.
	bool CheckOutCorner(int32& OutCornerIndex) const;

	// Checks for valid in-corner conditions.
	bool CheckInCorner();

	// Validates if a corner hop can be performed for in corners.
	bool CheckInCornerHop();

	// Validates if a ledge is valid for hop actions.
	bool CheckLedgeValid();

	// Performs hop action out of a corner.
	bool CheckOutCornerHop();


	////////////////////////////////////////////////////////////////////
	//
	//	CALCULATIONS
	//
	////////////////////////////////////////////////////////////////////

	// Returns the desired rotation based on control rotation and movement direction.
	FRotator GetDesiredRotation() const;

	// Finds the location for a dropdown hang.
	void FindDropDownHangLocation();

	// Finds the location for a hop action.
	void FindHopLocation();

	// Calculates forward values for climbing.
	void GetClimbForwardValue(float ScaleValue, float& HorizontalForwardValue, float& VerticalForwardValue) const;

	// Calculates right values for climbing.
	void GetClimbRightValue(float ScaleValue, float& HorizontalRightValue, float& VerticalRightValue) const;

	// Returns the vertical axis value for climbing.
	float GetVerticalAxis() const;

	// Returns the horizontal axis value for climbing.
	float GetHorizontalAxis() const;

	// Returns the desired rotation tag for climbing.
	FGameplayTag GetClimbDesireRotation();

	// Returns the speed for climbing movements.
	float GetClimbMoveSpeed() const;

	// Returns the Z offset for the left hand during climbing.
	float GetClimbLeftHandZOffset() const;

	// Returns the Z offset for the right hand during climbing.
	float GetClimbRightHandZOffset() const;

	////////////////////////////////////////////////////////////////////
	//
	//	GAMEPLAY TAG HANDLING
	//
	////////////////////////////////////////////////////////////////////

	// Determines the type of parkour action.
	void ParkourType(bool bAutoClimb);

	// Sets the current parkour action.
	void SetParkourAction(const FGameplayTag& NewParkourAction);

	// Sets the current parkour state.
	void SetParkourState(const FGameplayTag& NewParkourState);

	// Sets the current climb style.
	void SetClimbStyle(const FGameplayTag& NewClimbStyle);

	// Sets the current climb direction.
	void SetClimbDirection(const FGameplayTag& NewClimbDirection);

	// Configures parkour settings.
	void SetUpParkourSettings(ECollisionEnabled::Type CollsionType, EMovementMode MovementMode, FRotator RotationRate, bool bDoCollisionTest, bool bStopImmediately);

	// Selects the appropriate hop action based on tags.
	FGameplayTag SelectHopAction();

	// Returns the direction for hop actions.
	FGameplayTag GetHopDirection() const;

	////////////////////////////////////////////////////////////////////
	//
	//	DYNAMIC CAMERA HANDLING
	//
	////////////////////////////////////////////////////////////////////

	// Configures settings for transitioning between states.
	void PreviousStateSettings(const FGameplayTag& PreviousState, const FGameplayTag& NewState);

	// Adds a camera timeline for smooth transitions.
	void AddCameraTimeline(float Time);

	// Updates the camera timeline during transitions.
	void CameraTimelineTick();

	// Finalizes the camera timeline.
	void FinishTimeline();

	////////////////////////////////////////////////////////////////////
	//
	//	INVERSE KINEMATICS (IK)
	//
	////////////////////////////////////////////////////////////////////

	// Calculates results for the climb ledge.
	void ClimbLedgeResultCalculation(FHitResult& ClimbLedgeResult);

	// Calculates IK for the hand on the ledge.
	void HandLedgeIK(FHitResult& LedgeResult, bool bIsLeft);

	// Sets all values for the hand IK
	void SetHandIK(const FHitResult& FirstHitResult, const FHitResult& SecondHitResult, bool bIsLeft, bool bIsFinal);

	// Calculates IK for the foot.
	void SetFootIK(FHitResult& LedgeResult, bool bIsLeft, bool bIsSimplified = false);

	// Calculates IK for climbing movement.
	void ClimbMoveIK();

	// Calculates IK for the hands during climbing movement.
	void ClimbMoveHandIK();

	// Calculates IK for the  hand during climbing movement.
	void UpdateClimbMoveHandIK(bool bIsLeft);

	// Calculates IK for the feet during climbing movement.
	void ClimbMoveFootIK();

	// Calculates IK for the left foot during climbing movement.
	void UpdateClimbMoveFootIK(bool bIsLeft);

	// Resets IK for the feet.
	void ResetFootIK(bool bIsLeft);

	////////////////////////////////////////////////////////////////////
	//
	//	MONTAGE HANDLING
	//
	////////////////////////////////////////////////////////////////////

	// Plays the montage for parkour actions.
	void PlayParkourMontage();

	// Finds the location for warping based on impact points.
	FVector FindWarpLocation(const FVector& ImpactPoint, float XOffset, float ZOffset) const;

	// Checks and finds the warp location based on impact points.
	FVector FindWarpLocationChecked(const FVector& ImpactPoint, float XOffset, float ZOffset) const;

	// Finds the start time for the montage.
	void FindMontageStartTime();

	void PreinitializeParkourDataAssets(FParkourVariablesCollection& ParkourCollection) const;

	////////////////////////////////////////////////////////////////////
	//
	//	TRACING OPERATIONS
	//
	////////////////////////////////////////////////////////////////////

	// Performs a sphere trace by channel.
	void PerformSphereTraceByChannel(UWorld* World, FHitResult& HitResult, const FVector& StartLocation, const FVector& EndLocation,
		float Radius, ECollisionChannel TraceChannel, bool bDrawDebugSphere, float DrawTime = 0.5f) const;

	// Performs a box trace by channel.
	void PerformBoxTraceByChannel(UWorld* World, FHitResult& HitResult, const FVector& StartLocation, const FVector& EndLocation,
		const FVector& BoxHalfExtent, ECollisionChannel CollisionChannel, bool bDrawDebugBox, float DrawTime = 0.5f) const;

	// Performs a capsule trace by channel.
	void PerformCapsuleTraceByChannel(UWorld* World, FHitResult& HitResult, const FVector& StartLocation, const FVector& EndLocation,
		float HalfHeight, float Radius, ECollisionChannel CollisionChannel, bool bDrawDebugCapsule, float DrawTime = 0.5f) const;

	// Performs a line trace by channel.
	void PerformLineTraceByChannel(UWorld* World, FHitResult& HitResult, const FVector& StartLocation, const FVector& EndLocation,
		ECollisionChannel CollisionChannel, bool bDrawDebugLine, float DrawTime = 0.5f) const;

	// Displays hit results for debugging.
	void ShowHitResults();

	// Resets the results of parkour traces.
	void ResetParkourResults();

	////////////////////////////////////////////////////////////////////
	//
	//	VARIABLES AND REFERENCES
	//
	////////////////////////////////////////////////////////////////////

	UPROPERTY()
	TObjectPtr<ACharacter> Character;
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> CharacterMovement;
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> CharacterMesh;
	UPROPERTY()
	TObjectPtr<UAnimInstance> AnimInstance;
	UPROPERTY()
	TObjectPtr<UCapsuleComponent> CapsuleComponent;
	UPROPERTY()
	TObjectPtr<USpringArmComponent> CameraBoom;
	UPROPERTY()
	TObjectPtr<UMotionWarpingComponent> MotionWarping;
	UPROPERTY()
	TObjectPtr<UCameraComponent> Camera;
	UPROPERTY()
	TObjectPtr<UCurveFloat> CameraCurve;
	UPROPERTY()
	TObjectPtr<AWidgetActor> WidgetActor;
	UPROPERTY()
	TObjectPtr<AArrowActor> ArrowActor;
	UPROPERTY()
	TObjectPtr<UParkourVariables> ParkourVariables;

	FGameplayTag ParkourActionTag;
	FGameplayTag ParkourStateTag;
	FGameplayTag ClimbStyle;
	FGameplayTag ClimbDirection;

	bool bCanAutoClimb;
	bool bCanManualClimb;
	bool bOnGround;
	bool bShouldHandleCameraInput;

	float WallHeight;
	float WallDepth;
	float VaultHeight;

	float CameraCurveAlpha;
	float MontageStartTime;

	float HorizontalClimbForwardValue;
	float VerticalClimbForwardValue;
	float HorizontalClimbRightValue;
	float VerticalClimbRightValue;

	float FirstCameraTargetArmLenght;
	float TargetArmLength;
	float ForwardScale;
	float RightScale;

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

	FVector FirstTargetRelativeLocation;
	FVector TargetRelativeCameraLocation;
	FVector LeftHandLedgeLocation;
	FVector RightHandLedgeLocation;
	FRotator LeftHandLedgeRotation;
	FRotator RightHandLedgeRotation;

	FRotator WallRotation;
	FRotator CornerHopRotation;

	FTimerHandle TimerHandle_DelayedFunction;
	FTimerHandle TimerHandle_FinishCameraTimeline;
	FTimerHandle TimerHandle_TickCameraTimeline;

};