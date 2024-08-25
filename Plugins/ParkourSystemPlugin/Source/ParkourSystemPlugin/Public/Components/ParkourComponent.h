// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ParkourInterface.h"
#include "GameFramework/Character.h"
#include "WidgetActor.h"
#include "ArrowActor.h"
#include "GameplayTagContainer.h"
#include "Curves/CurveFloat.h"
#include "ParkourVariablesCollection.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ParkourComponent.generated.h"

struct FInputActionValue;
struct FParkourVariablesCollection;
class UParkourVariables;
class UInputMappingContext;
class UInputAction;

USTRUCT(BlueprintType)
struct FGeneralParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Threshold for determining if the horizontal axis is significant for climbing moves."))
	FVector WidgetActorPosition = FVector(100.0f, 50.0f, -3.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Threshold for determining if the horizontal axis is significant for climbing moves."))
	float BracedAutoClimbBoxCheckZ = 50.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Threshold for determining if the horizontal axis is significant for climbing moves."))
	float FreeAutoClimbBoxCheckZ = 2.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Threshold for determining if the horizontal axis is significant for climbing moves."))
	FVector AutoClimbBoxExtend = FVector(10.0f, 10.0f, 4.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Threshold for determining if the horizontal axis is significant for climbing moves."))
	float ManualClimbDropDelay = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Threshold for determining if the horizontal axis is significant for climbing moves."))
	FName RootSocketName = "root";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Threshold for determining if the horizontal axis is significant for climbing moves."))
	float BracedMoveSpeedMaxClamp = 98.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Threshold for determining if the horizontal axis is significant for climbing moves."))
	float FreeHangMoveSpeedMaxClamp = 55.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Threshold for determining if the horizontal axis is significant for climbing moves."))
	float BracedMoveSpeedMultiplier = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Threshold for determining if the horizontal axis is significant for climbing moves."))
	float FreeHangMoveSpeedMultiplier = 0.08f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bShouldDropOnBackwardHop = false;
};

USTRUCT(BlueprintType)
struct FClimbMoveParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Threshold for determining if the horizontal axis is significant for climbing moves."))
	float MoveHorizontalAxisThreshold = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for the outer loop checking climb move."))
	int32 OuterLoopIterationsNum = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size in Z direction for each iteration of the outer loop."))
	float OuterLoopStepZ = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance to check forward from the character during the climb move."))
	float ForwardCheckDistance = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Offset to the right of the character during the climb move."))
	float RightOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere trace used in the outer loop."))
	float SphereTraceRadiusOuter = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugOuter = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for the inner loop checking climb move."))
	int32 InnerLoopIterationsNum = 7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Offset forward for each iteration of the inner loop."))
	float ForwardOffset = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size in Z direction for each iteration of the inner loop."))
	float InnerLoopStepZ = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere trace used in the inner loop."))
	float SphereTraceRadiusInner = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the inner loop trace."))
	bool bDrawDebugInner = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for checking sides."))
	int32 NumSideChecks = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size in Z direction for each side check iteration."))
	float SideCheckStepZ = 9.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Initial Z offset for side checks."))
	float SideCheckInitialZOffset = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance to trace left/right for side checks."))
	float SideTraceDistance = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the trace for side checks."))
	float SideTraceRadius = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for side checks."))
	bool bDrawDebugSideCheck = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Speed for interpolating character rotation."))
	float RotationInterpSpeed = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "X style multiplier for braced climbing style."))
	float XStyleBraced = -44.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "X style multiplier for free climbing style."))
	float XStyleFree = -7.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Z style multiplier for braced climbing style."))
	float ZStyleBraced = 107.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Z style multiplier for free climbing style."))
	float ZStyleFree = 115.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Interpolation speed for Z axis in braced climbing style."))
	float ZInterpSpeedBraced = 2.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Interpolation speed for Z axis in free climbing style."))
	float ZInterpSpeedFree = 1.8f;
};

USTRUCT(BlueprintType)
struct FCornerMoveParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Interpolation speed for Z axis in free climbing style."))
	float OverTimeBraced = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Interpolation speed for Z axis in free climbing style."))
	float OverTimeFree = 0.9f;
};

USTRUCT(BlueprintType)
struct FOutCornerMoveParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Number of iterations for the loop to check the top of the wall."))
	float IndexMultiplierZOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Distance for the sphere trace to check for a wall on the side."))
	float SideTraceDistance = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Radius of the sphere trace for checking walls."))
	float SideTraceRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugSide = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Number of iterations for the loop to check the top of the wall."))
	int32 LoopIterationsNum = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Distance increments for checking the top of the wall."))
	float LoopDistanceIncrement = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Radius of the sphere trace for checking walls."))
	float LoopTraceRadius = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugLoop = false;

	// Same ClimbMoveParams same as there vvv
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Multiplier for calculating the target relative location based on climbing style."))
	float StyleMultiplierBraced = 44.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Multiplier for calculating the target relative location based on climbing style."))
	float StyleMultiplierFree = 7.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Value subtracted from the target Z calculation based on climbing style."))
	float TargetLocationZOffset = 107.0f;
};

USTRUCT(BlueprintType)
struct FCornerHopParams
{
	GENERATED_BODY()

	// Horizontal distance for the hop based on whether it's an outer corner or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float CornerHorizontalHopDistanceOut = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop when not an outer corner."))
	float CornerHorizontalHopDistanceIn = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Start index for the loop based on horizontal axis direction."))
	int32 OuterStartIndexLeft = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Start index for the loop when moving right."))
	int32 OuterStartIndexRight = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "End index for the loop when moving right."))
	int32 OuterNumOfIterations = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Distance for line traces in outer loop."))
	float OuterRightOffsetStep = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Distance for line traces in outer loop."))
	float OuterForwardOffset = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Distance for line traces in outer loop."))
	float OuterLoopForwardCheckDistance = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugOuter = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Distance to adjust start and end locations in inner loop."))
	float OuterLoopZAdjustment = 16.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Start index for the loop when moving right."))
	int32 InnerNumOfIterations = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugInner = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Step size for Z adjustments in inner loop."))
	float DistanceThreshold = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Step size for Z adjustments in inner loop."))
	float InnerLoopZStep = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Distance to trace capsule for checking fitting and collisions."))
	float CapsuleTraceForwardDistance = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Height of the capsule for collision checks."))
	float CapsuleHalfHeightAdjustment = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Height of the capsule for collision checks."))
	float ClosestCheckForwardOffset = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Height of the capsule for collision checks."))
	float ClosestCheckRightOffset = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Height of the capsule for collision checks."))
	float ClosestCheckRightDistance = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Height of the capsule for collision checks."))
	float ClosestCheckCapsuleRadius = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugCapsuleCheck = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Distance to adjust the start location of the wall top trace."))
	float TopCheckZDistance = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Height of the capsule for collision checks."))
	float TopCheckSpehreRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugTopCheck = false;
};

USTRUCT(BlueprintType)
struct FChekcWallShapeParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float CornerHorizontalHopDistanceOut = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop when not an outer corner."))
	float CornerHorizontalHopDistanceIn = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Start index for the loop based on horizontal axis direction."))
	int32 OuterNumOfIterationsFalling = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Start index for the loop when moving right."))
	int32 OuterNumOfIterationsDefault = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "End index for the loop when moving right."))
	int32 InnerNumOfIterations = 11;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Distance for line traces in outer loop."))
	float InnerInitialForwardOffset = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Distance for line traces in outer loop."))
	float InnerZOffsetStep = 16.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Distance for line traces in outer loop."))
	float InnerForwardOffsetStep = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Distance for line traces in outer loop."))
	float InnerInitialForwardDistance = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Distance for line traces in outer loop."))
	float InnerSphereRadius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugInner = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Start index for the loop when moving right."))
	int32 NumOfColumnIterationsDefault = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Start index for the loop when moving right."))
	int32 NumOfColumnIterationsClimb = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Distance to adjust start and end locations in inner loop."))
	float ColumnClimbZOffset = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Distance to adjust start and end locations in inner loop."))
	float ColumnClimbRightOffsetStep = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Distance to adjust start and end locations in inner loop."))
	float ColumnClimbInitialRightOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Distance to adjust start and end locations in inner loop."))
	float ColumnClimbInitialForwardOffset = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Distance to adjust start and end locations in inner loop."))
	float ColumnClimbForwardDistance = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugColumn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Start index for the loop when moving right."))
	int32 NumOfRowIterationsDefault = 29;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Start index for the loop when moving right."))
	int32 NumOfRowIterationsClimb = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Step size for Z adjustments in inner loop."))
	float RowZOffsetStep = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugRow = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Step size for Z adjustments in inner loop."))
	float DistanceThreshold = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Step size for Z adjustments in inner loop."))
	int32 NumOfTopCheckIterations = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Distance to trace capsule for checking fitting and collisions."))
	float TopCheckForwardOffsetStep = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Distance to trace capsule for checking fitting and collisions."))
	float TopCheckInitialForwardOffset = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Height of the capsule for collision checks."))
	float TopCheckZDistance = 7.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Height of the capsule for collision checks."))
	float TopCheckSphereRadius = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugTopCheck = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Height of the capsule for collision checks."))
	float DepthCheckForwardDistance = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Height of the capsule for collision checks."))
	float DepthCheckSphereRadius = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugDepthCheck = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Height of the capsule for collision checks."))
	float VaultCheckForwardDistance = 70.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Height of the capsule for collision checks."))
	float VaultCheckZOffset = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Height of the capsule for collision checks."))
	float VaultCheckSphereRadius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugVaultCheck = false;
};

USTRUCT(BlueprintType)
struct FCheckMantleSurfaceParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ZOffset = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop when not an outer corner."))
	float HalfHeightAdjustment = -8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Start index for the loop based on horizontal axis direction."))
	float CapsuleRadius = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebug = false;
};

USTRUCT(BlueprintType)
struct FCheckVaultSurfaceParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ZOffset = 18.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop when not an outer corner."))
	float HalfHeightAdjustment = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Start index for the loop based on horizontal axis direction."))
	float CapsuleRadius = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebug = false;
};

USTRUCT(BlueprintType)
struct FCheckClimbSurfaceParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ZOffset = -90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ForwardOffset = -55.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Start index for the loop based on horizontal axis direction."))
	float CapsuleRadius = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebug = false;
};

USTRUCT(BlueprintType)
struct FCheckClimbStyleParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ZOffset = 125.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ForwardOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ForwardCheckDistance = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Start index for the loop based on horizontal axis direction."))
	float SphereCheckRadius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebug = false;
};

USTRUCT(BlueprintType)
struct FFirstTraceHeightParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float NonClimbingPresetValue = -60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	int32 OuterNumOfiTerations = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	FName RightHandSocketName = "hand_r";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	FName LeftHandSocketName = "hand_l";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float OuterForwardOffset = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float OuterForwardCheckDistance = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float OuterSphereCheckRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugOuter = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	int32 InnerNumOfiTerations = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float InnerForwardOffset = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float InnerZInitialOffset = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float InnerZOffsetStep = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float InnerZCheckDistance = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Start index for the loop based on horizontal axis direction."))
	float InnerSphereCheckRadius = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugInner = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Start index for the loop based on horizontal axis direction."))
	float ResultZAdjustment = 4.0f;
};

USTRUCT(BlueprintType)
struct FCheckAirHangParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	FName HeadSocketName = "head";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float HeadLedgeThreshold = 30.0f;
};

USTRUCT(BlueprintType)
struct FCheckClimbMoveSurfaceParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float RightOffset = 13.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ForwardOffset = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ZOffset = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ForwardCheckDistance = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Start index for the loop based on horizontal axis direction."))
	float CapsuleCheckRadius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebug = false;
};

USTRUCT(BlueprintType)
struct FCheckOutCornerParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	int32 NumOfIterations = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	int32 StartIndex = -2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float RightOffset = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ZOffsetStep = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ForwardCheckDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Start index for the loop based on horizontal axis direction."))
	float SpehereCheckRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebug = false;
};

USTRUCT(BlueprintType)
struct FCheckInCornerParams
{
	GENERATED_BODY()

	// It is good to keep this one the same as FCheckOutCornerParams::NumOfIterations
	// vvv
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	int32 EndWallNumOfIterations = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float EndWallRightOffsetStep = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float EndWallForwardOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float EndWallForwardCheckDistance = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float EndWallSphereRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugEndWall = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	int32 SideNumOfIterations = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float SideForwardOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float SideRightOffset = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float SideZOffsetStep = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float SideRightCheckDistance = 70.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float SideSphereRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugSide = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	int32 TopNumOfIterations = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float TopForwardOffset = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float TopZOffsetStep = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float TopSphereRadius = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugTop = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float CapsuleForwardOffset = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float CapsuleRadius = 27.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugCapsule = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float TargetForwardMultiplierBraced = 44.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float TargetForwardMultiplierFreeHang = 9.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float TargetZOffsetBraced = 107.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float TargetZOffsetFreeHang = 118.0f;
};

USTRUCT(BlueprintType)
struct FCheckInCornerHopParams
{
	GENERATED_BODY()


	// It is good to keep this one the same as FCheckOutCornerParams::NumOfIterations
	// vvv
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	int32 EndWallNumOfIterations = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float EndWallRightOffsetStep = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float EndWallForwardOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float EndWallForwardCheckDistance = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float EndWallSphereRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugEndWall = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	int32 RotationNumOfIterations = 23;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float RotationForwardOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float RotationZOffsetStep = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float RotationRightOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float RotationRightCheckDistance = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float RotationSphereRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugRotation = false;
};

USTRUCT(BlueprintType)
struct FCheckOutCornerHopParams
{
	GENERATED_BODY()


	// It is good to keep this one the same as FCheckOutCornerParams::NumOfIterations
	// vvv
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	int32 NumOfIterations = 23;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ForwardOffset = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ZOffsetStep = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float RightOffset = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float RightCheckDistance = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float SphereRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebug = false;

};

USTRUCT(BlueprintType)
struct FFindDropDownHangLocationParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float BottomZOffset = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float BottomSphereRadius = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugBottom = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float WallForwardOffset = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float WallZOffset = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float WallForwardCheckDistance = 125.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float WallSphereRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugWall = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	int32 ColumnNumOfIterations = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ColumnInitialRightOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ColumnRightOffsetStep = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ColumnForwardOffset = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ColumnForwardCheckDistance = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugColumn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	int32 RowNumOfIterations = 12;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float RowZOffsetStep = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugRow = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float DistanceThreshold = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float TopForwardOffset = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float TopZDistanceCheck = 7.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float TopSphereRadius = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugTop = false;
};

USTRUCT(BlueprintType)
struct FFindHopLocationParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float VerticalForward = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float VerticalBackward = -7.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float VerticalSide = -2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float VerticalForwardSide = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float VerticalBackwardSide = -4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float HorizontalSide = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float HorizontalDiagonal = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float VerticalOffsetMultiplier = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float HorizontalOffsetMultiplier = 140.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	int32 ColumnNumOfIterations = 7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ColumnRightOffsetStep = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ColumnInitialForwardOffset = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ColumnForwardCheckDistance = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugColumn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	int32 RowNumOfIterations = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	float RowZOffsetStep = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugRow = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float DistanceThreshold = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float CapsuleHeightAdjustment = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float CapsuleForwardOffset = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float CapsuleRightOffset = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float CapsuleRightCheckDistance = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float CapsuleRadius = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugCapsule = false;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float TopZOffset = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float TopSphereRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugTop = false;
};

USTRUCT(BlueprintType)
struct FClimbHandZOffsetParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float LeftBracedSubtractForRight = 255.72f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float LeftBracedSubtractForLeft = 195.72f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float LeftFreeSubtractForRight = 229.09f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float LeftFreeSubtractForLeft = 227.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	FName LeftCurveName= "Hand_L Z";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float RightBracedSubtractForRight = 254.37f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float RightBracedSubtractForLeft = 194.36f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float RightFreeSubtractForRight = 227.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float RightFreeSubtractForLeft = 229.09f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	FName RightCurveName = "Hand_R Z";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float MaxClampOffset = 5.0f;
};

USTRUCT(BlueprintType)
struct FParkourTypeParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float WallHeightMin = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float WallHeightLevel1 = 44.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float WallHeightLevel2 = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float WallHeightLevel3 = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float WallHeightLevel4 = 160.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float WallHeightMax = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float VaultHeightLevel1 = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float VaultHeightLevel2 = 160.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float WallDepthMin = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float WallDepthLevel1 = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float WallDepthLevel2 = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float VaultVelocityThreshold = 20.0f;
};

USTRUCT(BlueprintType)
struct FHopDirectionParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ForwardBackwardThreshold = 37.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float SideThresholdMin = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float SideThresholdMax = 135.0f;

};

USTRUCT(BlueprintType)
struct FPreviousStateSettingsParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float CameraTimelineDuration = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ReachLedgeTargetArmLength = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	FVector ReachLedgeCameraLocation = FVector(-50.0f, 0.0f, 70.0f);

};

USTRUCT(BlueprintType)
struct FClimbLedgeResultCalculationParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float EdgeForwardCheckDistance = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float EdgeSpehreRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	bool bDrawDebugEdge = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float TopInitialForwardOffset = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float TopInitialZOffset = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float TopInitialZCheckDistance = 55.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float TopSpehreRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	bool bDrawDebugTop = false;
};

USTRUCT(BlueprintType)
struct FHandLedgeIKParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	int32 OuterNumOfIteration = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float OuterForwardOffset = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float OuterInitialRightOffset = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float OuterRightOffsetStep = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float OuterForwardCheckDIstance = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float OuterFSphereRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	bool bDrawDebugOuter = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	int32 InnerNumOfIteration = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float InnerForwardOffset = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float InnerZOffsetStep = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float InnerZCheckDistance = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float InnerSphereRadius = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	bool bDrawDebugInner = false;
};

USTRUCT(BlueprintType)
struct FSetHandIKParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ForwardOffset = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ZOffset = 9.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float LeftRoll = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float LeftPitch= -90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float RightRoll = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float RightPitch = 90.0f;
};

USTRUCT(BlueprintType)
struct FSetFootIKParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	int32 SimplifiedNumOfIterations = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	int32 DefaultNumOfIterations = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ForwardOffset = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float RightOffset = 7.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ZOffsetStep = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float InitialZOffset = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ForwardCheckDistance = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float SphereRadius= 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	bool bDrawDebug= false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ResultForwardOffset = 17.0f;
};

USTRUCT(BlueprintType)
struct FUpdateClimbMoveFootIKParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	FName LeftFootCurveName = "LeftFootIK";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	FName RightFootCurveName = "RightFootIK";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	FName LeftFootIKSocketName = "ik_foot_l";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	FName RightFootIKSocketName = "ik_foot_r";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	FName LeftHandSocketName = "hand_l";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	FName RightHandSocketName = "hand_r";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float LeftUpOffset = 135.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float RightUpOffset = 125.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	int32 OuterNumOfIterations = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	int32 InnerNumOfIterations = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float LoopZOffsetStep = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float LoopInitialRightOffset= 13.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float LoopRightOffsetStep = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float LoopForwardOffset = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float LoopForwardCheckDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float LoopSphereRadius= 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	bool bDrawDebugLoop = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ResultForwardOffset = 18.0f;
};

USTRUCT(BlueprintType)
struct FUpdateClimbMoveHandIKParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	FName LeftHandIKSocketName = "ik_hand_l";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	FName RightHandIKSocketName = "ik_hand_r";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	int32 FrontNumOfIterations = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float FrontRightOffsetStep = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float FrontForwardOffset = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float FrontForwardCheckDistance = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float FrontSphereRadius= 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	bool bDrawDebugFront = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	int32 TopNumOfIterations = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float TopInitialZOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float TopZOffsetStep = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float TopSphereRadius = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	bool bDrawDebugTop = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ResultForwardOffset = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ResultZOffsetBraced = 9.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ResultZOffsetFree = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float LeftHandRoll = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float LeftHandPitch = -90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float RightHandRoll = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float RightHandPitch = 90.0f;
};

USTRUCT(BlueprintType)
struct FFindWarpLocationCheckedParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ZOffset = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float ZDistanceCheck = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	float SphereRadius = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour Movement", meta = (Tooltip = "Horizontal distance for the hop, modified based on whether it's an outer corner or not."))
	bool bDrawDebug = false;
};

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
	FChekcWallShapeParams ChekcWallShapeParams;

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
	virtual bool SetInitializeReference(ACharacter* NewCharacter, USpringArmComponent* NewCameraBoom, UMotionWarpingComponent* NewMotionWarping, UCameraComponent* NewCamera) override;

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

	float ArrowLocationX;
	float ArrowLocationZ;
	float CharacterHeightDiff;
	float CharacterHandUp;
	float CharacterHandFront;
	float FirstCameraTargetArmLenght;
	float TargetArmLength;
	float ForwardScale;
	float RightScale;

	FGameplayTag ParkourActionTag;
	FGameplayTag ParkourStateTag;
	FGameplayTag ClimbStyle;
	FGameplayTag ClimbDirection;

	bool bCanAutoClimb;
	bool bCanManualClimb;
	bool bOnGround;

	float WallHeight;
	float WallDepth;
	float VaultHeight;

	float CameraCurveAlpha;
	float MontageStartTime;

	float HorizontalClimbForwardValue;
	float VerticalClimbForwardValue;
	float HorizontalClimbRightValue;
	float VerticalClimbRightValue;

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