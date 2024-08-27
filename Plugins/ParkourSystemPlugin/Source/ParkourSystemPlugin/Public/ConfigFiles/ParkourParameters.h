
#pragma once

#include "CoreMinimal.h"
#include "ParkourParameters.generated.h"

// How naing works for default floats?
// First part is what segment of a check parameter is. For example, TopZOffsetStep - Top means that the check is doint Top of the wall
// It could be Top for top of the wall check, Side for checking alongside of the wall, Bottom for checking what is under, Column for line traces that represents columns and row in the same manner
// 
// Second part is usually Z, Right or Forward. It means that this parameter will be changed.
// Third part is in what way certain parameter is changed. It could be: _Offset, Initial_Offset, _OffsetStep, _CheckDistance. Where _ is a parameter


USTRUCT(BlueprintType)
struct FGeneralParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Widget relative position to character."))
	FVector WidgetActorPosition = FVector(100.0f, 50.0f, -3.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Z offset of auto climb box check for braced style."))
	float BracedAutoClimbBoxCheckZ = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Z offset of auto climb box check  for free hang style."))
	float FreeAutoClimbBoxCheckZ = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Box extend for asuto climb check."))
	FVector AutoClimbBoxExtend = FVector(10.0f, 10.0f, 4.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the auto climb box trace."))
	bool bDrawAutoClimbBox = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Delay that blocks manual climb after drop."))
	float ManualClimbDropDelay = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Root socket name of character."))
	FName RootSocketName = "root";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Maximum possible speed for braced climb move."))
	float BracedMoveSpeedMaxClamp = 98.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Maximum possible speed for free hang climb move."))
	float FreeHangMoveSpeedMaxClamp = 55.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Speed multiplier for braced climb move."))
	float BracedMoveSpeedMultiplier = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Speed multiplier for free hang climb move."))
	float FreeHangMoveSpeedMultiplier = 0.08f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether character should drop down on backward hop move, if there is no space to hop."))
	bool bShouldDropOnBackwardHop = false;
};

USTRUCT(BlueprintType)
struct FClimbMoveParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Threshold for determining if the horizontal axis is significant for climbing moves."))
	float MoveHorizontalAxisThreshold = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for the outer loop checking climb move."))
	int32 OuterNumOfIterations = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size in Z direction for each iteration of the outer loop."))
	float OuterZOffsetStep = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance to check forward from the character during the climb move."))
	float ForwardCheckDistance = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Offset to the right of the character during the climb move."))
	float RightOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere trace used in the outer loop."))
	float SphereTraceRadiusOuter = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugOuter = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for the inner loop checking climb move."))
	int32 InnerNumOfIterations = 7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Offset forward for each iteration of the inner loop."))
	float ForwardOffset = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size in Z direction for each iteration of the inner loop."))
	float InnerZOffsetStep = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere trace used in the inner loop."))
	float SphereTraceRadiusInner = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the inner loop trace."))
	bool bDrawDebugInner = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for checking sides."))
	int32 SideNumOfIteration = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size in Z direction for each side check iteration."))
	float SideZOffsetStep = 9.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Initial Z offset for side checks."))
	float SideInitialZOffset = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance to trace left/right for side checks."))
	float SideRightCheckDistance = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the trace for side checks."))
	float SideSpehreRadius = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for side checks."))
	bool bDrawDebugSide = false;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "When should be time over for braced style."))
	float OverTimeBraced = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "When should be time over for free hang style."))
	float OverTimeFree = 0.9f;
};

USTRUCT(BlueprintType)
struct FOutCornerMoveParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset for corner checks, used for adjustment free hang style."))
	float ArrowForwardOffsetStyleFree = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Z-axis offset for corner checks, multiplied by the index during each iteration."))
	float IndexMultiplierZOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance for checking the right side during a corner move."))
	float SideRightCheckDistance = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere trace used to check for walls on the side during a corner move."))
	float SideSphereRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to visualize side check traces during corner moves."))
	bool bDrawDebugSide = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for looping through vertical checks during a corner move."))
	int32 LoopNumOfIterations = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Vertical step size applied to Z-offset for each loop iteration when checking the top of the wall."))
	float LoopZOffsetStep = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere trace used to check the top of the wall during a corner move."))
	float LoopSphereRadius = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to visualize top check traces during the loop in corner moves."))
	bool bDrawDebugLoop = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Multiplier for calculating the target relative location based on braced climbing style."))
	float StyleMultiplierBraced = 44.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Multiplier for calculating the target relative location based on free climbing style."))
	float StyleMultiplierFree = 7.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Z-axis offset subtracted from the target location based on the selected climbing style."))
	float TargetLocationZOffset = 107.0f;
};

USTRUCT(BlueprintType)
struct FCornerHopParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Horizontal distance for the hop for an outer corner."))
	float CornerHorizontalHopDistanceOut = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Horizontal distance for the hop for an inner corner."))
	float CornerHorizontalHopDistanceIn = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Start index for the outer loop when moving left around a corner."))
	int32 OuterStartIndexLeft = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Start index for the outer loop when moving right around a corner."))
	int32 OuterStartIndexRight = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for the outer loop."))
	int32 OuterNumOfIterations = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Rightward offset increment for each step of the outer loop."))
	float OuterRightOffsetStep = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset for the starting point of the outer loop."))
	float OuterForwardOffset = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward distance for line traces in the outer loop when moving around a corner."))
	float OuterLoopForwardCheckDistance = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugOuter = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Z-axis adjustment applied during the outer loop for start and end locations."))
	float OuterLoopZAdjustment = 16.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for the inner loop when moving around a corner."))
	int32 InnerNumOfIterations = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the inner loop trace."))
	bool bDrawDebugInner = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance threshold for detecting valid hop locations."))
	float DistanceThreshold = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size for Z-axis adjustments during the inner loop."))
	float InnerLoopZStep = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward distance for capsule trace during collision fitting checks."))
	float CapsuleTraceForwardDistance = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Adjustment to the capsule's half-height during collision checks."))
	float CapsuleHalfHeightAdjustment = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset for checking the closest point during the hop."))
	float ClosestCheckForwardOffset = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Rightward offset for checking the closest point during the hop."))
	float ClosestCheckRightOffset = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Rightward distance for the closest rightward check during the hop."))
	float ClosestRightCheckDistance = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the capsule used for checking the closest point during the hop."))
	float ClosestCheckCapsuleRadius = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the capsule check during the hop."))
	bool bDrawDebugCapsuleCheck = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance to adjust the Z-axis during top wall checks."))
	float TopZCheckDistance = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used for checking the top of the wall."))
	float TopCheckSphereRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the top check trace during the hop."))
	bool bDrawDebugTopCheck = false;
};

USTRUCT(BlueprintType)
struct FCheckWallShapeParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for the outer loop when falling."))
	int32 OuterNumOfIterationsFalling = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Default number of iterations for the outer loop."))
	int32 OuterNumOfIterationsDefault = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for the inner loop."))
	int32 InnerNumOfIterations = 11;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Initial forward offset for the inner loop trace."))
	float InnerInitialForwardOffset = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size for Z-axis adjustments in the inner loop."))
	float InnerZOffsetStep = 16.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset increment for each step of the inner loop."))
	float InnerForwardOffsetStep = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance for line traces in the inner loop."))
	float InnerForwardCheckDistance = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used for inner loop checks."))
	float InnerSphereRadius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the inner loop trace."))
	bool bDrawDebugInner = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Default number of iterations for column checks."))
	int32 NumOfColumnIterationsDefault = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for column checks during climbing."))
	int32 NumOfColumnIterationsClimb = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Z-axis offset for column checks during climbing."))
	float ColumnClimbZOffset = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Rightward offset increment for column checks during climbing."))
	float ColumnClimbRightOffsetStep = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Initial rightward offset for column checks during climbing."))
	float ColumnClimbInitialRightOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Initial forward offset for column checks during climbing."))
	float ColumnClimbInitialForwardOffset = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward check distance for column checks during climbing."))
	float ColumnClimbForwardCheckDistance = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for column checks."))
	bool bDrawDebugColumn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Default number of iterations for row checks."))
	int32 NumOfRowIterationsDefault = 29;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for row checks during climbing."))
	int32 NumOfRowIterationsClimb = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for row checks while falling."))
	int32 NumOfRowIterationsFalling = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Z-axis offset increment for each step of row checks."))
	float RowZOffsetStep = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for row checks."))
	bool bDrawDebugRow = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance threshold for detecting valid wall shapes."))
	float DistanceThreshold = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for top checks."))
	int32 NumOfTopCheckIterations = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset increment for top checks."))
	float TopCheckForwardOffsetStep = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Initial forward offset for top checks."))
	float TopCheckInitialForwardOffset = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Z-axis distance for top checks."))
	float TopCheckZDistance = 7.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used for top checks."))
	float TopCheckSphereRadius = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for top checks."))
	bool bDrawDebugTopCheck = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward distance for depth checks."))
	float DepthCheckForwardDistance = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used for depth checks."))
	float DepthCheckSphereRadius = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for depth checks."))
	bool bDrawDebugDepthCheck = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward distance for vault checks."))
	float VaultCheckForwardDistance = 70.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Z-axis offset for vault checks."))
	float VaultCheckZOffset = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used for vault checks."))
	float VaultCheckSphereRadius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for vault checks."))
	bool bDrawDebugVaultCheck = false;
};

USTRUCT(BlueprintType)
struct FCheckMantleSurfaceParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Z-axis offset for checking the mantle surface."))
	float ZOffset = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Adjustment to the half-height when checking the mantle surface."))
	float HalfHeightAdjustment = -8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the capsule used for checking the mantle surface."))
	float CapsuleRadius = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the mantle surface check."))
	bool bDrawDebug = false;
};

USTRUCT(BlueprintType)
struct FCheckVaultSurfaceParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Vertical offset for adjusting the start location of the vault check."))
	float ZOffset = 18.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Adjustment value for the capsule half-height during the vault check."))
	float HalfHeightAdjustment = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the capsule used in the vault surface check."))
	float CapsuleRadius = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the vault surface check."))
	bool bDrawDebug = false;
};

USTRUCT(BlueprintType)
struct FCheckClimbSurfaceParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Vertical offset for adjusting the start location of the climb check."))
	float ZOffset = -90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset for adjusting the start location of the climb check."))
	float ForwardOffset = -55.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the capsule used in the climb surface check."))
	float CapsuleRadius = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the climb surface check."))
	bool bDrawDebug = false;
};

USTRUCT(BlueprintType)
struct FCheckClimbStyleParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Vertical offset for adjusting the start location based on climb style."))
	float ZOffset = 125.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset for adjusting the start location based on climb style."))
	float ForwardOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance for forward checks based on climb style."))
	float ForwardCheckDistance = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used in climb style checks."))
	float SphereCheckRadius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the climb style check."))
	bool bDrawDebug = false;
};

USTRUCT(BlueprintType)
struct FFirstTraceHeightParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Preset value used for non-climbing scenarios to adjust the first trace height."))
	float NonClimbingPresetValue = -60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for the outer loop during the trace height check."))
	int32 OuterNumOfIterations = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Name of the socket for the right hand during the trace height check."))
	FName RightHandSocketName = "hand_r";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Name of the socket for the left hand during the trace height check."))
	FName LeftHandSocketName = "hand_l";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset for adjusting the start location of the outer loop trace."))
	float OuterForwardOffset = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward check distance for the outer loop trace."))
	float OuterForwardCheckDistance = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used in the outer loop trace check."))
	float OuterSphereCheckRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer loop trace."))
	bool bDrawDebugOuter = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for the inner loop during the trace height check."))
	int32 InnerNumOfIterations = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset for adjusting the start location of the inner loop trace."))
	float InnerForwardOffset = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Initial vertical offset for the inner loop trace."))
	float InnerZInitialOffset = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size for vertical offset increments in the inner loop trace."))
	float InnerZOffsetStep = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Vertical check distance for the inner loop trace."))
	float InnerZCheckDistance = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used in the inner loop trace check."))
	float InnerSphereCheckRadius = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the inner loop trace."))
	bool bDrawDebugInner = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Z adjustment applied to the final result after the trace height checks."))
	float ResultZAdjustment = 4.0f;
};

USTRUCT(BlueprintType)
struct FCheckAirHangParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Name of the socket used to determine the head position during air hang."))
	FName HeadSocketName = "head";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Threshold distance for determining if the head is close to a ledge during an air hang."))
	float HeadLedgeThreshold = 30.0f;
};

USTRUCT(BlueprintType)
struct FCheckClimbMoveSurfaceParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Offset to the right for adjusting the start location of the climb move check."))
	float RightOffset = 13.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset for adjusting the start location of the climb move check."))
	float ForwardOffset = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Vertical offset for adjusting the start location of the climb move check."))
	float ZOffset = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance for forward checks during the climb move."))
	float ForwardCheckDistance = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the capsule used in the climb move surface check."))
	float CapsuleCheckRadius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the climb move surface check."))
	bool bDrawDebug = false;
};

USTRUCT(BlueprintType)
struct FCheckOutCornerParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for checking the outer corner."))
	int32 NumOfIterations = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Initial index for the outer corner check loop."))
	int32 StartIndex = -2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Offset to the forward for adjusting the start location of the outer corner check."))
	float InitialForwardOffset = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Offset to the right for adjusting the start location of the outer corner check."))
	float RightOffset = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size for vertical offset increments during the outer corner check."))
	float ZOffsetStep = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance for forward checks during the outer corner check."))
	float ForwardCheckDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used in the outer corner check."))
	float SphereCheckRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer corner check."))
	bool bDrawDebug = false;
};

USTRUCT(BlueprintType)
struct FCheckInCornerParams
{
	GENERATED_BODY()

	// It is good to keep this one the same as FCheckOutCornerParams::NumOfIterations
	// vvv
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for checking the end wall of an inner corner."))
	int32 EndWallNumOfIterations = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size for right offset adjustments in the end wall check."))
	float EndWallRightOffsetStep = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset for adjusting the end wall check position."))
	float EndWallForwardOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance for forward checks during the end wall check."))
	float EndWallForwardCheckDistance = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used in the end wall check."))
	float EndWallSphereRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the end wall check."))
	bool bDrawDebugEndWall = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for checking the side of an inner corner."))
	int32 SideNumOfIterations = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset for adjusting the side check position."))
	float SideForwardOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Right offset for adjusting the side check position."))
	float SideRightOffset = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size for vertical offset adjustments in the side check."))
	float SideZOffsetStep = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance for right checks during the side check."))
	float SideRightCheckDistance = 70.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used in the side check."))
	float SideSphereRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the side check."))
	bool bDrawDebugSide = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for checking the top of an inner corner."))
	int32 TopNumOfIterations = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset for adjusting the top check position."))
	float TopForwardOffset = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size for vertical offset adjustments in the top check."))
	float TopZOffsetStep = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used in the top check."))
	float TopSphereRadius = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the top check."))
	bool bDrawDebugTop = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset for adjusting the capsule check position."))
	float CapsuleForwardOffset = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the capsule used in the check."))
	float CapsuleRadius = 27.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the capsule check."))
	bool bDrawDebugCapsule = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Multiplier for the target forward offset when braced."))
	float TargetForwardMultiplierBraced = 44.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Multiplier for the target forward offset when free hanging."))
	float TargetForwardMultiplierFreeHang = 9.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Vertical offset for the target when braced."))
	float TargetZOffsetBraced = 107.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Vertical offset for the target when free hanging."))
	float TargetZOffsetFreeHang = 118.0f;
};

USTRUCT(BlueprintType)
struct FCheckInCornerHopParams
{
	GENERATED_BODY()

	// It is good to keep this one the same as FCheckOutCornerParams::NumOfIterations
	// vvv
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for checking the end wall during an inner corner hop."))
	int32 EndWallNumOfIterations = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size for right offset adjustments during an inner corner hop."))
	float EndWallRightOffsetStep = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset for adjusting the end wall check position during an inner corner hop."))
	float EndWallForwardOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance for forward checks during the end wall check in an inner corner hop."))
	float EndWallForwardCheckDistance = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used in the end wall check during an inner corner hop."))
	float EndWallSphereRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the end wall check during an inner corner hop."))
	bool bDrawDebugEndWall = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for the rotation check during an inner corner hop."))
	int32 RotationNumOfIterations = 23;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset for adjusting the rotation check position during an inner corner hop."))
	float RotationForwardOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size for vertical offset adjustments during the rotation check in an inner corner hop."))
	float RotationZOffsetStep = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Right offset for adjusting the rotation check position during an inner corner hop."))
	float RotationRightOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance for right checks during the rotation check in an inner corner hop."))
	float RotationRightCheckDistance = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used in the rotation check during an inner corner hop."))
	float RotationSphereRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the rotation check during an inner corner hop."))
	bool bDrawDebugRotation = false;
};

USTRUCT(BlueprintType)
struct FCheckOutCornerHopParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for checking the outer corner during a hop."))
	int32 NumOfIterations = 23;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset for adjusting the outer corner hop check position."))
	float ForwardOffset = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size for vertical offset adjustments during the outer corner hop check."))
	float ZOffsetStep = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Right offset for adjusting the outer corner hop check position."))
	float RightOffset = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance for right checks during the outer corner hop check."))
	float RightCheckDistance = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used in the outer corner hop check."))
	float SphereRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer corner hop check."))
	bool bDrawDebug = false;
};

USTRUCT(BlueprintType)
struct FFindDropDownHangLocationParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Vertical offset from the bottom to check for potential hang locations."))
	float BottomZOffset = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used to detect drop-down hang locations from the bottom."))
	float BottomSphereRadius = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the bottom check."))
	bool bDrawDebugBottom = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset from the wall to check for potential hang locations."))
	float WallForwardOffset = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Vertical offset from the wall to check for potential hang locations."))
	float WallZOffset = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance to check forward from the wall for potential hang locations."))
	float WallForwardCheckDistance = 125.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used to detect hang locations near the wall."))
	float WallSphereRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the wall check."))
	bool bDrawDebugWall = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for checking columns in potential hang locations."))
	int32 ColumnNumOfIterations = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Initial right offset for column checks."))
	float ColumnInitialRightOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size for adjusting the right offset in column checks."))
	float ColumnRightOffsetStep = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset for column checks."))
	float ColumnForwardOffset = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance to check forward in column checks."))
	float ColumnForwardCheckDistance = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the column checks."))
	bool bDrawDebugColumn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for checking rows in potential hang locations."))
	int32 RowNumOfIterations = 12;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size for adjusting the vertical offset in row checks."))
	float RowZOffsetStep = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the row checks."))
	bool bDrawDebugRow = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Threshold distance for determining valid drop-down hang locations."))
	float DistanceThreshold = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset for top checks in drop-down hang location detection."))
	float TopForwardOffset = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance to check vertically for top checks in drop-down hang location detection."))
	float TopZCheckDistance = 7.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used in top checks for drop-down hang location detection."))
	float TopSphereRadius = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the top checks."))
	bool bDrawDebugTop = false;
};

USTRUCT(BlueprintType)
struct FFindHopLocationParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Vertical forward offset for hop location calculations."))
	float VerticalForward = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Vertical backward offset for hop location calculations."))
	float VerticalBackward = -9.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Vertical side offset for hop location calculations."))
	float VerticalSide = -2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Vertical forward side offset for hop location calculations."))
	float VerticalForwardSide = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Vertical backward side offset for hop location calculations."))
	float VerticalBackwardSide = -7.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Horizontal side offset for hop location calculations."))
	float HorizontalSide = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Horizontal diagonal offset for hop location calculations."))
	float HorizontalDiagonal = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Multiplier for vertical offset adjustments in hop location calculations."))
	float VerticalOffsetMultiplier = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Multiplier for horizontal offset adjustments in hop location calculations for braced style."))
	float HorizontalOffsetMultiplierBraced = 140.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Multiplier for horizontal offset adjustments in hop location calculations for free hang style."))
	float HorizontalOffsetMultiplierFree = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for checking columns in hop location calculations."))
	int32 ColumnNumOfIterationsBraced = 7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for checking columns in hop location calculations."))
	int32 ColumnNumOfIterationsFree = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size for adjusting right offset in column checks during hop location calculations."))
	float ColumnRightOffsetStep = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Initial forward offset for column checks during hop location calculations."))
	float ColumnInitialForwardOffset = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance to check forward in column checks during hop location calculations."))
	float ColumnForwardCheckDistance = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for column checks during hop location calculations."))
	bool bDrawDebugColumn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for checking rows in hop location calculations."))
	int32 RowNumOfIterations = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size for adjusting vertical offset in row checks during hop location calculations."))
	float RowZOffsetStep = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for row checks during hop location calculations."))
	bool bDrawDebugRow = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Threshold distance for determining valid hop locations."))
	float DistanceThreshold = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Vertical adjustment for capsule height in hop location calculations."))
	float CapsuleHeightAdjustment = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset for capsule checks during hop location calculations."))
	float CapsuleForwardOffset = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Right offset for capsule checks during hop location calculations."))
	float CapsuleRightOffset = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the capsule used in hop location calculations."))
	float CapsuleRadius = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for capsule checks during hop location calculations."))
	bool bDrawDebugCapsule = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Vertical offset for top checks in hop location calculations."))
	float TopZOffset = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used in top checks for hop location calculations."))
	float TopSphereRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for top checks during hop location calculations."))
	bool bDrawDebugTop = false;
};

USTRUCT(BlueprintType)
struct FClimbHandZOffsetParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Subtract value for left hand when braced for right climbing."))
	float LeftBracedSubtractForRight = 255.72f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Subtract value for left hand when braced for left climbing."))
	float LeftBracedSubtractForLeft = 195.72f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Subtract value for left hand when free climbing for right climbing."))
	float LeftFreeSubtractForRight = 229.09f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Subtract value for left hand when free climbing for left climbing."))
	float LeftFreeSubtractForLeft = 227.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Curve name for left hand Z offset adjustments."))
	FName LeftCurveName = "Hand_L Z";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Subtract value for right hand when braced for right climbing."))
	float RightBracedSubtractForRight = 254.37f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Subtract value for right hand when braced for left climbing."))
	float RightBracedSubtractForLeft = 194.36f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Subtract value for right hand when free climbing for right climbing."))
	float RightFreeSubtractForRight = 227.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Subtract value for right hand when free climbing for left climbing."))
	float RightFreeSubtractForLeft = 229.09f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Curve name for right hand Z offset adjustments."))
	FName RightCurveName = "Hand_R Z";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Maximum clamp offset for Z adjustments."))
	float MaxClampOffset = 5.0f;
};

USTRUCT(BlueprintType)
struct FParkourTypeParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Minimum height of walls for parkour actions."))
	float WallHeightMin = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Height of walls at Level 1 for parkour actions. (Used for Low Mantle)"))
	float WallHeightLevel1 = 44.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Height of walls at Level 2 for parkour actions. (Used for Thin Vault, default Vailt, default Mantle)"))
	float WallHeightLevel2 = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Height of walls at Level 3 for parkour actions. (Used for Hight Vault, default Mantle)"))
	float WallHeightLevel3 = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Height of walls at Level 4 for parkour actions. (Used for Climbing)"))
	float WallHeightLevel4 = 160.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Maximum height of walls for parkour actions. "))
	float WallHeightMax = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Vault height requirement at Level 1 for parkour actions. (Used for Thin Vault and default Vault)"))
	float VaultHeightLevel1 = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Vault height requirement at Level 2 for parkour actions. (Used for Hight Vault)"))
	float VaultHeightLevel2 = 160.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Minimum depth of walls for parkour actions."))
	float WallDepthMin = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Depth of walls at Level 1 for parkour actions. (Used for Thin Vault)"))
	float WallDepthLevel1 = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Depth of walls at Level 2 for parkour actions. (Used for Vault)"))
	float WallDepthLevel2 = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Minimum velocity required to initiate a vault during parkour actions."))
	float VaultVelocityThreshold = 20.0f;
};

USTRUCT(BlueprintType)
struct FHopDirectionParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Threshold for determining forward or backward hop directions."))
	float ForwardBackwardThreshold = 37.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Minimum threshold for determining side hop directions."))
	float SideThresholdMin = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Maximum threshold for determining side hop directions."))
	float SideThresholdMax = 135.0f;
};

USTRUCT(BlueprintType)
struct FPreviousStateSettingsParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Duration of the camera timeline transition in seconds."))
	float CameraTimelineDuration = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Camera arm lenght during reach ledge action."))
	float ReachLedgeTargetArmLength = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Camera relative location during reach ledge action."))
	FVector ReachLedgeCameraLocation = FVector(-50.0f, 0.0f, 70.0f);
};

USTRUCT(BlueprintType)
struct FClimbLedgeResultCalculationParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance to check forward from the edge for ledge climbing."))
	float EdgeForwardCheckDistance = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used for detecting ledge edges."))
	float EdgeSphereRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for edge checks during climbing."))
	bool bDrawDebugEdge = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Initial forward offset for top edge checks in ledge climbing."))
	float TopInitialForwardOffset = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Initial vertical offset for top edge checks in ledge climbing."))
	float TopInitialZOffset = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance to check vertically from the top edge for ledge climbing."))
	float TopInitialZCheckDistance = 55.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used for detecting top edges."))
	float TopSpehreRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for top checks during climbing."))
	bool bDrawDebugTop = false;
};

USTRUCT(BlueprintType)
struct FHandLedgeIKParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for the outer hand IK search."))
	int32 OuterNumOfIteration = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset distance for the outer hand IK search."))
	float OuterForwardOffset = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Initial right offset for the outer hand IK search."))
	float OuterInitialRightOffset = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size for increasing the right offset in the outer hand IK search."))
	float OuterRightOffsetStep = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance to check forward in the outer hand IK search."))
	float OuterForwardCheckDIstance = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used in the outer hand IK search."))
	float OuterFSphereRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the outer hand IK search."))
	bool bDrawDebugOuter = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for the inner hand IK search."))
	int32 InnerNumOfIteration = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset distance for the inner hand IK search."))
	float InnerForwardOffset = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size for increasing the Z offset in the inner hand IK search."))
	float InnerZOffsetStep = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance to check vertically in the inner hand IK search."))
	float InnerZCheckDistance = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used in the inner hand IK search."))
	float InnerSphereRadius = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the inner hand IK search."))
	bool bDrawDebugInner = false;
};

USTRUCT(BlueprintType)
struct FSetHandIKParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset for hand IK positioning."))
	float ForwardOffset = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Vertical offset for hand IK positioning."))
	float ZOffset = 9.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Roll angle for the left hand IK."))
	float LeftRoll = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Pitch angle for the left hand IK."))
	float LeftPitch = -90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Roll angle for the right hand IK."))
	float RightRoll = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Pitch angle for the right hand IK."))
	float RightPitch = 90.0f;
};

USTRUCT(BlueprintType)
struct FSetFootIKParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for simplified foot IK search."))
	int32 SimplifiedNumOfIterations = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for default foot IK search."))
	int32 DefaultNumOfIterations = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset distance for foot IK positioning."))
	float ForwardOffset = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Right offset distance for foot IK positioning."))
	float RightOffset = 7.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size for increasing the Z offset in foot IK positioning."))
	float ZOffsetStep = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Initial vertical offset for foot IK positioning."))
	float InitialZOffset = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance to check forward in foot IK positioning."))
	float ForwardCheckDistance = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used in foot IK positioning."))
	float SphereRadius = 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the foot IK positioning."))
	bool bDrawDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset applied to the result of the foot IK positioning."))
	float ResultForwardOffset = 17.0f;
};

USTRUCT(BlueprintType)
struct FUpdateClimbMoveFootIKParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Name of the animation curve for the left foot IK."))
	FName LeftFootCurveName = "LeftFootIK";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Name of the animation curve for the right foot IK."))
	FName RightFootCurveName = "RightFootIK";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Name of the socket for the left foot IK."))
	FName LeftFootIKSocketName = "ik_foot_l";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Name of the socket for the right foot IK."))
	FName RightFootIKSocketName = "ik_foot_r";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Name of the socket for the left hand IK."))
	FName LeftHandSocketName = "hand_l";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Name of the socket for the right hand IK."))
	FName RightHandSocketName = "hand_r";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Vertical offset for the left foot IK."))
	float LeftUpOffset = 135.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Vertical offset for the right foot IK."))
	float RightUpOffset = 125.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for the outer foot IK search."))
	int32 OuterNumOfIterations = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for the inner foot IK search."))
	int32 InnerNumOfIterations = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size for increasing the Z offset in foot IK search."))
	float LoopZOffsetStep = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Initial right offset for the loop in foot IK search."))
	float LoopInitialRightOffset = 13.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size for increasing the right offset in foot IK search."))
	float LoopRightOffsetStep = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset distance for the loop in foot IK search."))
	float LoopForwardOffset = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance to check forward in foot IK search."))
	float LoopForwardCheckDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used in the foot IK search."))
	float LoopSphereRadius = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the foot IK search loop."))
	bool bDrawDebugLoop = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset applied to the result of the foot IK search."))
	float ResultForwardOffset = 18.0f;
};

USTRUCT(BlueprintType)
struct FUpdateClimbMoveHandIKParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Name of the socket for the left hand IK."))
	FName LeftHandIKSocketName = "ik_hand_l";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Name of the socket for the right hand IK."))
	FName RightHandIKSocketName = "ik_hand_r";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for the front hand IK search."))
	int32 FrontNumOfIterations = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size for increasing the right offset in the front hand IK search."))
	float FrontRightOffsetStep = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset distance for the front hand IK search."))
	float FrontForwardOffset = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance to check forward in the front hand IK search."))
	float FrontForwardCheckDistance = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used in the front hand IK search."))
	float FrontSphereRadius = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the front hand IK search."))
	bool bDrawDebugFront = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Number of iterations for the top hand IK search."))
	int32 TopNumOfIterations = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Initial Z offset for the top hand IK search."))
	float TopInitialZOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Step size for increasing the Z offset in the top hand IK search."))
	float TopZOffsetStep = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used in the top hand IK search."))
	float TopSphereRadius = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the top hand IK search."))
	bool bDrawDebugTop = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Forward offset applied to the result of the hand IK search."))
	float ResultForwardOffset = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Z offset for the result when braced during hand IK search."))
	float ResultZOffsetBraced = 9.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Z offset for the result when free during hand IK search."))
	float ResultZOffsetFree = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Roll angle for the left hand IK."))
	float LeftHandRoll = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Pitch angle for the left hand IK."))
	float LeftHandPitch = -90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Roll angle for the right hand IK."))
	float RightHandRoll = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Pitch angle for the right hand IK."))
	float RightHandPitch = 90.0f;
};

USTRUCT(BlueprintType)
struct FFindWarpLocationCheckedParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Vertical offset for finding warp locations."))
	float ZOffset = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Distance to check vertically for finding warp locations."))
	float ZCheckDistance = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Radius of the sphere used in finding warp locations."))
	float SphereRadius = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether to draw debug information for the warp location check."))
	bool bDrawDebug = false;
};