// Fill out your copyright notice in the Description page of Project Settings.


#include "ParkourComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameplayTagsManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Curves/CurveFloat.h"
#include "Animation/AnimInstance.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimTypes.h"

#include "ParkourFunctionLibrary.h"
#include "ParkourABPInterface.h"
#include "ParkourStatsInterface.h"
#include "ParkourVariables.h"


UParkourComponent::UParkourComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Serach for Camera curve and set reference
	FString path = "/ParkourSystemPlugin/Curves/FC_ParkourCameraMove";
	static ConstructorHelpers::FObjectFinder<UCurveFloat> MontageAsset(*path);

	if (MontageAsset.Succeeded())
		CameraCurve = MontageAsset.Object;
	else
		UE_LOG(LogTemp, Warning, TEXT("UParkourComponent: CameraCurve wasn't found"));

	// Set up all default tags
	ParkourActionTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.NoAction"));
	ParkourStateTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.State.NotBusy"));
	ClimbStyle = UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.ClimbStyle.FreeHang"));
	FGameplayTag NoDirectionTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.NoDirection"));
	ClimbDirection = NoDirectionTag;

	// Set up all default variables
	ClimbHandSpace = 20.0f;
	bCanAutoClimb = true;
	bCanManualClimb = true;
	bDrawDebug = false;
	bOnGround = true;

	PreinitializeParkourDataAssets(ParkourVariablesCollection);
}

void UParkourComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

void UParkourComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Check whether auto-climb should be enable
	AutoClimb();

	// Automatically activate IK when moving while climbing 
	ClimbMoveIK();
}

bool UParkourComponent::SetInitializeReference(ACharacter* NewCharacter, USpringArmComponent* NewCameraBoom, UMotionWarpingComponent* NewMotionWarping, UCameraComponent* NewCamera)
{
	// Initialize all character related references
	Character = NewCharacter;
	CharacterMovement = Character->GetCharacterMovement();
	CharacterMesh = Character->GetMesh();
	AnimInstance = CharacterMesh->GetAnimInstance();
	CapsuleComponent = Character->GetCapsuleComponent();
	CameraBoom = NewCameraBoom;
	MotionWarping = NewMotionWarping;
	Camera = NewCamera;

	if (Character)
	{
		// Create widget actor and attach it to the character
		WidgetActor = Character->GetWorld()->SpawnActor<AWidgetActor>(AWidgetActor::StaticClass(), Character->GetActorLocation(), FRotator::ZeroRotator);
		if (WidgetActor)
		{
			FAttachmentTransformRules AttachmentRules(
				EAttachmentRule::SnapToTarget,
				EAttachmentRule::SnapToTarget,
				EAttachmentRule::SnapToTarget,
				true
			);
			WidgetActor->AttachToComponent(Camera, AttachmentRules);
			WidgetActor->SetActorRelativeLocation(WidgetActorPosition);
		}
		else
			return false;

		// Create arrow actor and attach it to the character
		ArrowActor = Character->GetWorld()->SpawnActor<AArrowActor>(AArrowActor::StaticClass(), Character->GetActorLocation(), FRotator::ZeroRotator);
		if (ArrowActor)
		{
			FAttachmentTransformRules AttachmentRules(
				EAttachmentRule::KeepRelative,
				EAttachmentRule::KeepRelative,
				EAttachmentRule::KeepRelative,
				true
			);
			ArrowActor->AttachToComponent(CharacterMesh, AttachmentRules);

			ArrowLocationX = 0.0f;
			ArrowLocationZ = 195.0f;
			CharacterHeightDiff = 0.0f;
			ArrowActor->SetActorRelativeLocation(FVector(ArrowLocationX, 0.0f, ArrowLocationZ - CharacterHeightDiff));
		}
		else
			return false;


		// Set up all input actions within input mapping context
		if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(ParkourMappingContext, 1);

				if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
				{
					EnhancedInputComponent->BindAction(ParkourInputAction, ETriggerEvent::Started, this, &UParkourComponent::ParkourAction);
					EnhancedInputComponent->BindAction(ParkourDropInputAction, ETriggerEvent::Started, this, &UParkourComponent::ParkourDrop);
					EnhancedInputComponent->BindAction(ParkourMoveInputAction, ETriggerEvent::Triggered, this, &UParkourComponent::Move);
					EnhancedInputComponent->BindAction(ParkourMoveInputAction, ETriggerEvent::Completed, this, &UParkourComponent::ResetMovement);
				}
			}
		}

	}
	else
		return false;

	// Bind OnParkourMontageBlendOut method to the character animation instance
	if (AnimInstance)
		AnimInstance->OnMontageBlendingOut.AddDynamic(this, &UParkourComponent::OnParkourMontageBlendOut);

	// Initialize camera boom and related fields
	if (CameraBoom)
	{
		FirstCameraTargetArmLenght = CameraBoom->TargetArmLength;
		FirstTargetRelativeLocation = CameraBoom->GetRelativeLocation();
	}
	else
		return false;

	return true;
}

void UParkourComponent::ParkourAction()
{
	// Call parkour action function with auto-climb diactivated (is used to invoke via manual input action)
	ParkourActionFunction(false);
}

void UParkourComponent::ParkourActionFunction(bool bAutoClimb)
{
	// Check is character currently doing anything
	if (!ParkourActionTag.GetTagName().IsEqual("Parkour.Action.NoAction"))
		return;

	// Check is character able to auto-climb or manual climb
	bool bCheckClimb = bAutoClimb ? bCanAutoClimb : bCanManualClimb;

	if (!bCheckClimb)
		return;

	// Check wall shape for climbing 
	ChekcWallShape();

	// Show hit results for debug purposes
	if (bDrawDebug)
		ShowHitResults();

	// Check and correct distances and invoke the parkour action itself
	CheckDistance();
	ParkourType(bAutoClimb);
}

void UParkourComponent::AutoClimb()
{
	FVector Location = CharacterMesh->GetSocketLocation("root");

	if (ParkourStateTag.GetTagName().IsEqual("Parkour.State.Climb"))
		Location.Z += UParkourFunctionLibrary::SelectClimbStyleFloat(BracedAutoClimbBoxCheckZ, FreeAutoClimbBoxCheckZ, ClimbStyle);

	// Trace box to check whether character is on ground 
	FHitResult HitResult;
	PerformBoxTraceByChannel(Character->GetWorld(), HitResult, Location, Location, AutoClimbBoxExtend, ECC_Visibility, bDrawDebug, 0.0f);
	bOnGround = (HitResult.bBlockingHit || HitResult.bStartPenetrating);

	// Determine character`s ability to manually climb and auto-climb 
	// If cahracter is on ground, then both types of climbing are available and parkour results are reseted
	// If not, parkour action function invoked with auto-climb parameter as true
	if (bOnGround)
	{
		if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.NoAction"))
		{
			bCanManualClimb = true;
			bCanAutoClimb = true;
			ResetParkourResults();
		}
	}
	else
		if (ParkourStateTag.GetTagName().IsEqual("Parkour.State.NotBusy"))
			ParkourActionFunction(true);
}

void UParkourComponent::ParkourDrop()
{
	// If character is not on ground and is climbing, firstly stop climbing and then allow manual climb with a delay
	// If character is, then parkour drop down action is used (firstly find a drop down location)
	if (!bOnGround)
	{
		if (ParkourStateTag.GetTagName().IsEqual("Parkour.State.Climb"))
		{
			// Set state to NotBusy, which stops climbing
			SetParkourState(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.State.NotBusy")));
			bCanManualClimb = false;
			bCanAutoClimb = false;

			FTimerManager& TimerManager = GetWorld()->GetTimerManager();

			TimerManager.SetTimer(TimerHandle_DelayedFunction, this, &UParkourComponent::SetCanManualClimb, ManualClimbDropDelay, false);
		}
	}
	else
	{
		// Check whether character is doing anything
		if (ParkourStateTag.GetTagName().IsEqual("Parkour.State.NotBusy"))
		{
			FindDropDownHangLocation();
		}
	}
}

void UParkourComponent::SetCanManualClimb()
{
	bCanManualClimb = true;
}

void UParkourComponent::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	// Check is character controller valid
	if (Character->GetController() == nullptr)
		return;

	ForwardScale = MovementVector.Y;
	RightScale = MovementVector.X;

	// Calculate forward and right value using sin and cos (value is between -1 and 1)
	GetClimbForwardValue(ForwardScale, HorizontalClimbForwardValue, VerticalClimbForwardValue);
	GetClimbRightValue(RightScale, HorizontalClimbRightValue, VerticalClimbRightValue);

	// If character doesn't any action, then add movement input
	// If does and it is climbing, then invoke climb move function
	if (ParkourStateTag.GetTagName().IsEqual("Parkour.State.NotBusy"))
	{
		const FRotator Rotation = Character->GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		Character->AddMovementInput(ForwardDirection, MovementVector.Y);
		Character->AddMovementInput(RightDirection, MovementVector.X);
	}
	else if (ParkourStateTag.GetTagName().IsEqual("Parkour.State.Climb"))
	{
		// Check whether character is playing any montages
		if (AnimInstance->IsAnyMontagePlaying())
			StopClimbMovement();
		else
			ClimbMove();
	}
}

void UParkourComponent::ClimbMove()
{
	// Check is character perfoming corner move
	if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.CornerMove"))
		return;

	// Check is Horizontal axis value big enought to move (determined by camera angle and input movement)
	if (FMath::Abs(GetHorizontalAxis()) <= ClimbMoveParams.MoveHorizontalAxisThreshold)
	{
		StopClimbMovement();
		return;
	}

	// Determine and set whther it is right or left movement
	FName NewDirectionString = GetHorizontalAxis() > 0.0f ? "Parkour.Direction.Right" : "Parkour.Direction.Left";
	SetClimbDirection(UGameplayTagsManager::Get().RequestGameplayTag(FName(NewDirectionString)));

	bool bShouldBreak = false;
	FHitResult OuterLoopHitResult;
	FHitResult InnerLoopHitResult;
	FVector StartLocation;
	FVector EndLocation;

	// Outer loop that checks wall in front of the character
	for (int32 i = 0; i < ClimbMoveParams.OuterLoopIterationsNum; i++)
	{
		FVector RightVector = ArrowActor->GetArrowComponent()->GetComponentRotation().RotateVector(FVector::RightVector);
		FVector ForwardVector = ArrowActor->GetArrowComponent()->GetComponentRotation().RotateVector(FVector::ForwardVector);

		StartLocation = ArrowActor->GetArrowComponent()->GetComponentLocation() + (ClimbMoveParams.RightOffset * GetHorizontalAxis() * RightVector);
		StartLocation.Z -= i * ClimbMoveParams.OuterLoopStepZ;
		EndLocation = StartLocation + ForwardVector * ClimbMoveParams.ForwardCheckDistance;

		// Trace sphere to check whether there is a wall in front of character from arrow perspective
		// Start gets lower after each iteration
		OuterLoopHitResult;
		PerformSphereTraceByChannel(Character->GetWorld(), OuterLoopHitResult, StartLocation, EndLocation, ClimbMoveParams.SphereTraceRadiusOuter, ECC_Visibility, ClimbMoveParams.bDrawDebugOuter, 0.0f);

		// If hit result is penetrating at start, then skip to next iteration
		if (OuterLoopHitResult.bStartPenetrating)
			continue;

		// Initialize index to use in check function for out corner and after that pass it to move function itslef
		int32 OutCornerIndex;
		if (CheckOutCorner(OutCornerIndex))
		{
			OutCornerMove(OutCornerIndex);
			break;
		}

		//  If hit result is not blocked by anything and it is not last iteration, then skip to next iteration
		if (!OuterLoopHitResult.bBlockingHit)
		{
			if (i != ClimbMoveParams.OuterLoopIterationsNum - 1)
				continue;

			// Check for in corner move if it is the last iteration
			CheckInCorner();
			break;
		}

		// Inner loop that checks top of the wall
		for (int32 j = 0; j < ClimbMoveParams.InnerLoopIterationsNum; j++)
		{
			FRotator ReversedRotator = UParkourFunctionLibrary::NormalReverseRotationZ(OuterLoopHitResult.ImpactNormal);
			ForwardVector = ReversedRotator.RotateVector(FVector::ForwardVector);

			StartLocation = OuterLoopHitResult.ImpactPoint + ForwardVector * ClimbMoveParams.ForwardOffset;
			StartLocation.Z += ClimbMoveParams.InnerLoopStepZ * (j + 1);

			EndLocation = StartLocation;
			EndLocation.Z -= ClimbMoveParams.InnerLoopStepZ * (j + 10);

			// Trace sphere to check whether there is the top of the edge
			// Start gets higher after each iteration
			PerformSphereTraceByChannel(Character->GetWorld(), InnerLoopHitResult, StartLocation, EndLocation, ClimbMoveParams.SphereTraceRadiusInner, ECC_Visibility, ClimbMoveParams.bDrawDebugInner, 0.0f);

			// If hit result is penetrating at start, then skip to next iteration
			if (InnerLoopHitResult.bStartPenetrating)
			{
				// Check is it the last possible iteration
				if (i == ClimbMoveParams.OuterLoopIterationsNum - 1 && j == ClimbMoveParams.InnerLoopIterationsNum - 1)
				{
					StopClimbMovement();
					return;
				}

				continue;
			}

			// If hit result was blocked by something, break both inner and outer loops
			if (InnerLoopHitResult.bBlockingHit)
				bShouldBreak = true;
			else
				StopClimbMovement();

			break;
		}

		// Check whether outer loop should break
		if (bShouldBreak)
			break;
	}

	// If outer loop was ended naturally and not breaked, then end function
	if (!bShouldBreak)
		return;

	// Loop that checks whether there is any blocking object on left/right side from top of the wall
	for (int32 i = 0; i < ClimbMoveParams.NumSideChecks; i++)
	{
		FRotator ReversedRotator = UParkourFunctionLibrary::NormalReverseRotationZ(OuterLoopHitResult.ImpactNormal);
		FVector RightVector = ReversedRotator.RotateVector(FVector::RightVector);
		FVector ForwardVector = ReversedRotator.RotateVector(FVector::ForwardVector);

		StartLocation = OuterLoopHitResult.ImpactPoint + ForwardVector * ClimbMoveParams.ForwardOffset;
		StartLocation.Z += i * ClimbMoveParams.SideCheckStepZ + ClimbMoveParams.SideCheckInitialZOffset;
		EndLocation = StartLocation + RightVector * GetHorizontalAxis() * ClimbMoveParams.SideTraceDistance;

		// Trace line to determine whether there is any blocking object on left/right side from top of the wall
		// Start gets higher after each iteration
		FHitResult LineTraceHitResult;
		PerformSphereTraceByChannel(Character->GetWorld(), LineTraceHitResult, StartLocation, EndLocation, ClimbMoveParams.SideTraceRadius, ECC_Visibility, ClimbMoveParams.bDrawDebugSideCheck, 0.0f);

		// If something is blocking hit result and it is the last iteration, then end function
		// If not, then break the loop
		if (!LineTraceHitResult.bBlockingHit)
		{
			// If climb surface is valid for moving, then set new wall rotation
			if (CheckClimbMoveSurface(OuterLoopHitResult))
				WallRotation = ReversedRotator;
			else
			{
				StopClimbMovement();
				return;
			}

			break;
		}
		else if (i == ClimbMoveParams.NumSideChecks - 1)
		{
			StopClimbMovement();
			return;
		}
	}

	// Initialize needed variables for character's move
	FRotator ReversedRotator = UParkourFunctionLibrary::NormalReverseRotationZ(OuterLoopHitResult.ImpactNormal);
	FVector ForwardVector = ReversedRotator.RotateVector(FVector::ForwardVector);
	FVector NewCharacterLocation;
	FRotator NewCharacterRotation;

	// Calculate target base vector based on first wall point, its forward vector and style multiplier 
	float StyleMultiplier = UParkourFunctionLibrary::SelectClimbStyleFloat(ClimbMoveParams.XStyleBraced, ClimbMoveParams.XStyleFree, ClimbStyle);
	FVector OuterLoopVector = OuterLoopHitResult.ImpactPoint + ForwardVector * StyleMultiplier;

	// Claculate target Z vector based on wall top point, character height diff and style subtracted value
	float StyleSub = UParkourFunctionLibrary::SelectClimbStyleFloat(ClimbMoveParams.ZStyleBraced, ClimbMoveParams.ZStyleFree, ClimbStyle);
	float TargetInterpZ = InnerLoopHitResult.ImpactPoint.Z + CharacterHeightDiff - StyleSub;

	// Interpolate character old position to new one with use of move speed
	float DeltaSeconds = Character->GetWorld()->GetDeltaSeconds();
	float StyleInterpSpeed = UParkourFunctionLibrary::SelectClimbStyleFloat(ClimbMoveParams.ZInterpSpeedBraced, ClimbMoveParams.ZInterpSpeedFree, ClimbStyle);
	NewCharacterLocation.X = FMath::FInterpTo(Character->GetActorLocation().X, OuterLoopVector.X, DeltaSeconds, GetClimbMoveSpeed());
	NewCharacterLocation.Y = FMath::FInterpTo(Character->GetActorLocation().Y, OuterLoopVector.Y, DeltaSeconds, GetClimbMoveSpeed());
	NewCharacterLocation.Z = FMath::FInterpTo(Character->GetActorLocation().Z, TargetInterpZ, DeltaSeconds, StyleInterpSpeed);

	// Interpolate character old rotation to new one 
	NewCharacterRotation = FMath::RInterpTo(Character->GetActorRotation(), WallRotation, DeltaSeconds, ClimbMoveParams.RotationInterpSpeed);
	Character->SetActorLocationAndRotation(NewCharacterLocation, NewCharacterRotation);

	// Call function to check and set needed climb style on move
	CheckClimbStyle(InnerLoopHitResult, WallRotation);
}

void UParkourComponent::HopAction()
{
	ClimbLedgeResultCalculation(FirstClimbLedgeResult);
	FindHopLocation();
	if (CheckLedgeValid())
	{
		CheckClimbStyle(WallTopResult, WallRotation);
		ClimbLedgeResultCalculation(SecondClimbLedgeResult);
		SetParkourAction(SelectHopAction());
	}
}

void UParkourComponent::CornerMove(const FVector& TargerRelativeLocation, const FRotator& TargerRelativeRotation)
{
	// Set appropriate tag
	SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.CornerMove")));

	// Set and determine which direction is going to be corner Move
	FName NewDirectionName = GetHorizontalAxis() > 0.0f ? "Parkour.Direction.Right" : "Parkour.Direction.Left";
	SetClimbDirection(UGameplayTagsManager::Get().RequestGameplayTag(FName(NewDirectionName)));

	// Set time depending on climb style
	float OverTimeByStyle = UParkourFunctionLibrary::SelectClimbStyleFloat(CornerMoveParams.OverTimeBraced, CornerMoveParams.OverTimeFree, ClimbStyle);

	// Initialize needed variables for Latent action info
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.Linkage = 0;
	LatentInfo.UUID = 1;
	LatentInfo.ExecutionFunction = FName("OnCornerMoveCompleted");

	// Call MoveComponentTo to change character's position
	UKismetSystemLibrary::MoveComponentTo(CapsuleComponent, TargerRelativeLocation, TargerRelativeRotation, false, false,
		OverTimeByStyle, true, EMoveComponentAction::Type::Move, LatentInfo);
}

void UParkourComponent::OutCornerMove(const int32& OutCornerIndex)
{
	FVector StartLocation;
	FVector EndLocation;
	FHitResult CheckHitResult;
	FHitResult LoopHitResult;

	FVector RightVector = ArrowActor->GetArrowComponent()->GetComponentRotation().RotateVector(FVector::RightVector);

	StartLocation = ArrowActor->GetArrowComponent()->GetComponentLocation();
	StartLocation.Z -= OutCornerIndex * OutCornerMoveParams.IndexMultiplierZOffset;
	EndLocation = StartLocation + RightVector * (GetHorizontalAxis() * OutCornerMoveParams.SideTraceDistance);

	// Trace sphere to check whether there is a wall on right/left side 
	PerformSphereTraceByChannel(Character->GetWorld(), CheckHitResult, StartLocation, EndLocation, OutCornerMoveParams.SideTraceRadius, ECC_Visibility, OutCornerMoveParams.bDrawDebugSide, 1.f);

	// If no wall was detected, then stop function
	if (!CheckHitResult.bBlockingHit)
		return;

	// Loop to get top of the wall
	for (int32 i = 0; i < OutCornerMoveParams.LoopIterationsNum; i++)
	{
		StartLocation = CheckHitResult.ImpactPoint;
		StartLocation.Z += OutCornerMoveParams.LoopDistanceIncrement * (i + 1);

		EndLocation = StartLocation;
		EndLocation.Z -= OutCornerMoveParams.LoopDistanceIncrement * (i + 10);

		// Trace sphere to get top of the wall from front side of the wall
		// Start gets higher after each iteration
		PerformSphereTraceByChannel(Character->GetWorld(), LoopHitResult, StartLocation, EndLocation, OutCornerMoveParams.LoopTraceRadius, ECC_Visibility, OutCornerMoveParams.bDrawDebugLoop, 0.f);

		// If hit result is not penetrated at start, then break the loop
		if (!LoopHitResult.bStartPenetrating)
			break;

		// If hit result was always penetrated at start, then stop function
		if (i == OutCornerMoveParams.LoopIterationsNum - 1)
		{
			StopClimbMovement();
			return;
		}
	}

	// If hit result is not blocked ny anything, then stop fucntion
	if (!LoopHitResult.bBlockingHit)
	{
		StopClimbMovement();
		return;
	}

	// Calculate TargetRelativeRotation and TargetRelativeRotation (the last one using StyleMultiplier)
	FRotator TargetRelativeRotation = UParkourFunctionLibrary::NormalReverseRotationZ(CheckHitResult.ImpactNormal);
	FVector ForwardVector = TargetRelativeRotation.RotateVector(FVector::ForwardVector);

	float StyleMultiplier = UParkourFunctionLibrary::SelectClimbStyleFloat(OutCornerMoveParams.StyleMultiplierBraced, OutCornerMoveParams.StyleMultiplierFree, ClimbStyle);
	FVector TargetRelativeLocation = CheckHitResult.ImpactPoint - ForwardVector * StyleMultiplier;
	TargetRelativeLocation.Z = LoopHitResult.ImpactPoint.Z - OutCornerMoveParams.TargetLocationZOffset;

	// Call CornerMove function with target location and rotation as parameters
	CornerMove(TargetRelativeLocation, TargetRelativeRotation);
}

void UParkourComponent::CornerHop(bool bIsOutCorner)
{
	// Determine hop distance and side
	float CornerHorizontalHopDistance = bIsOutCorner ? 50.0f : 20.0f;
	CornerHorizontalHopDistance *= GetHorizontalAxis();

	int32 FirstIndex = GetHorizontalAxis() < 0.0f ? 0 : 4;
	int32 LastIndex = GetHorizontalAxis() < 0.0f ? 3 : 7;

	// Clean up WallHitTraces from previous records
	WallHitTraces.Empty();

	// Outer loop that checks ...
	for (int32 i = FirstIndex; i < LastIndex; i++)
	{

		FVector CharacterUpVector = Character->GetActorRotation().RotateVector(FVector::UpVector);
		FVector CharacterRightVector = Character->GetActorRotation().RotateVector(FVector::RightVector);

		FVector WallRightVector = WallRotation.RotateVector(FVector::RightVector);
		FVector WallForwardVector = WallRotation.RotateVector(FVector::ForwardVector);

		FVector StartLocation = WallTopResult.ImpactPoint;
		StartLocation += CharacterUpVector * VerticalHopDistance;
		StartLocation += CharacterRightVector * CornerHorizontalHopDistance;
		StartLocation += WallRightVector * 20.0f * (i - 3);
		StartLocation -= WallForwardVector * 60.0f;

		FVector EndLocation = StartLocation + WallForwardVector * 120.0f;

		// Trace line to check whether there is a wall on the needed side 
		// Start moves to right/left after each iteration
		FHitResult OuterLoopTraceResult;
		PerformLineTraceByChannel(Character->GetWorld(), OuterLoopTraceResult, StartLocation, EndLocation, ECC_Visibility, bDrawDebug);

		//  Clean up HopHitTraces from previous iteration
		HopHitTraces.Empty();

		StartLocation.Z -= 16.0f;
		EndLocation.Z -= 16.0f;

		// First inner loop to get hit traces that show wall shape
		for (int32 k = 0; k <= 20; k++)
		{
			StartLocation.Z += 8.0f;
			EndLocation.Z += 8.0f;

			// Trace line to check wall shape
			// Start gets higher after each iteration
			FHitResult InnerLoopTraceResult;
			PerformLineTraceByChannel(Character->GetWorld(), InnerLoopTraceResult, StartLocation, EndLocation, ECC_Visibility, bDrawDebug);

			// If line trace is penetrating at start, then skip to next iteration
			if (InnerLoopTraceResult.bStartPenetrating)
				continue;

			// Add hit result to array
			HopHitTraces.Add(InnerLoopTraceResult);
		}

		// Second inner loop to determine nearest edge point
		for (int32 j = 1; j < HopHitTraces.Num(); j++)
		{
			// Check whether distance between to hit points big enough to consider it
			float Distance1 = HopHitTraces[j].bBlockingHit ? HopHitTraces[j].Distance : FVector::Distance(HopHitTraces[j].TraceStart, HopHitTraces[j].TraceEnd);
			float Distance2 = HopHitTraces[j - 1].bBlockingHit ? HopHitTraces[j - 1].Distance : FVector::Distance(HopHitTraces[j - 1].TraceStart, HopHitTraces[j - 1].TraceEnd);

			if (Distance1 - Distance2 <= 5.0f)
				continue;

			FRotator ReversedRotator = UParkourFunctionLibrary::NormalReverseRotationZ(HopHitTraces[j - 1].ImpactNormal);
			FVector ForwardVector = ReversedRotator.RotateVector(FVector::ForwardVector);
			FVector RightVector = ReversedRotator.RotateVector(FVector::RightVector);

			float CapsuleHalfHeight = CapsuleComponent->GetUnscaledCapsuleHalfHeight() - 20.0f;

			FVector CapsuleStartLocation = HopHitTraces[j - 1].ImpactPoint - ForwardVector * 40.0f + RightVector * 4.0f;
			CapsuleStartLocation.Z -= CapsuleHalfHeight;

			FVector CapsuleEndLocation = CapsuleStartLocation - RightVector * 8.0f;

			// Trace capsule to check whether character will fit and won't collide with anything
			FHitResult CapsuleCheckTrace;
			PerformCapsuleTraceByChannel(Character->GetWorld(), CapsuleCheckTrace, CapsuleStartLocation, CapsuleEndLocation, CapsuleHalfHeight, 25.0f, ECC_Visibility, bDrawDebug);

			// If point passes the check (capsule trace is not blocked by anything), then add it to array
			if (!CapsuleCheckTrace.bBlockingHit)
				WallHitTraces.Add(HopHitTraces[j - 1]);

			break;

		}
	}

	// Check is WallHitTraces is valid 
	if (!CheckLedgeValid())
		return;

	// Take first result from array as a start
	WallHitResult = WallHitTraces[0];

	// Loop to determine nearest to character point
	for (int32 i = 1; i < WallHitTraces.Num(); i++)
	{
		float Distance1 = FVector::Distance(Character->GetActorLocation(), WallHitTraces[i].ImpactPoint);
		float Distance2 = FVector::Distance(Character->GetActorLocation(), WallHitResult.ImpactPoint);
		if (Distance1 <= Distance2)
			WallHitResult = WallHitTraces[i];
	}

	// If optimal hit result is blocked at start, then stop function
	if (WallHitResult.bStartPenetrating)
		return;

	// Set new WallRotation
	if (!ParkourStateTag.GetTagName().IsEqual("Parkour.State.Climb"))
		WallRotation = UParkourFunctionLibrary::NormalReverseRotationZ(WallHitResult.ImpactNormal);

	FVector StartLocation = WallHitResult.ImpactPoint;
	StartLocation.Z += 3.0f;
	FVector EndLocation = StartLocation;
	EndLocation.Z -= 3.0f;

	// Trace sphere to have final check whether there is a top of the new edge
	FHitResult HitResult;
	PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug);

	// If hit result is not blocked by anything, then there is no top of the edge
	if (!HitResult.bBlockingHit)
		return;

	// Set new wall top result
	WallTopResult = HitResult;
}

void UParkourComponent::OnCornerMoveCompleted()
{
	SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.NoAction")));
}

void UParkourComponent::StopClimbMovement()
{
	// Stop movement and set direction tag to NoDirection
	CharacterMovement->StopMovementImmediately();
	SetClimbDirection(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.NoDirection")));
}

void UParkourComponent::ResetMovement()
{
	// Set both forward and right scales to 0 and set direction tag to NoDirection
	ForwardScale = 0.0f;
	RightScale = 0.0f;
	SetClimbDirection(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.NoDirection")));
}

void UParkourComponent::ChekcWallShape()
{
	//Initialize all needed variables for further use
	FHitResult HitResult;
	FHitResult LoopTraceHitResult;
	FHitResult InnerLoopTraceHitResult;

	FVector StartLocation;
	FVector EndLocation;

	//If character is falling make less iterations
	int32 Index = CharacterMovement->IsFalling() ? 8 : 15;



	bool bShouldBreak = false;
	for (int32 i = 0; i <= Index; i++)
	{
		for (int32 j = 0; j <= 11; j++)
		{
			StartLocation = Character->GetActorLocation() + FVector(0.0f, 0.0f, (i * 16.0f) + FirstTraceHeight()) -
				(Character->GetActorForwardVector() * (20.0f));
			EndLocation = StartLocation + (Character->GetActorForwardVector() * (j * 10.0f + 30.0f));

			// Trace sphere to get wall in front of the character and its height
			// Start gets higher after each iteration of outer loop
			// End gets further away after each iteration of inner loop
			PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 10.0f, ECC_Visibility, bDrawDebug, 0.0f);

			if (HitResult.bBlockingHit && !HitResult.bStartPenetrating)
			{
				bShouldBreak = true;
				break;
			}
		}

		if (bShouldBreak)
			break;
	}

	// If there is no wall in front or playe
	if (!HitResult.bBlockingHit || HitResult.bStartPenetrating)
		return;

	// Clear all previous records of wall traces
	WallHitTraces.Empty();


	// After getting wall in front of character, its shape should be checked by tracing forward lines
	// Basically it will be a "table" first loop traces line that will be first in the column
	// Second loop traces all lines by columns 

	// If character is climbing make less iterations (columns)
	Index = UParkourFunctionLibrary::SelectParkourStateFloat(4.0f, .0f, .0f, 2.0f, ParkourStateTag);

	// Loop to record all valid wall line traces (this loop represents columns)
	for (int32 i = 0; i <= Index; i++)
	{
		// Get impact point from previous check
		// If character is climbing then make ofset lower
		FVector ImpactPoint = HitResult.ImpactPoint;
		if (!ParkourStateTag.GetTagName().IsEqual("Parkour.State.Climb"))
			ImpactPoint.Z = Character->GetActorLocation().Z - 60.0f;

		FRotator ReversedRotator = UParkourFunctionLibrary::NormalReverseRotationZ(HitResult.ImpactNormal);
		FVector RightVector = ReversedRotator.RotateVector(FVector::RightVector);
		FVector ForwardVector = ReversedRotator.RotateVector(FVector::ForwardVector);

		// Right offset that depends on amount of iterations (columns)
		float Offset = (i * 2.0f - Index) * 10.0f;

		StartLocation = (RightVector * Offset) + ImpactPoint + (ForwardVector * -40.0f);
		EndLocation = (RightVector * Offset) + ImpactPoint + (ForwardVector * 30.0f);

		// Trace line that will indicate first in the column
		PerformLineTraceByChannel(Character->GetWorld(), LoopTraceHitResult, StartLocation, EndLocation, ECC_Visibility, bDrawDebug);

		// Clean Hop traces for each iteration (column)
		HopHitTraces.Empty();

		// Add first line trace in column to take it into account as well
		HopHitTraces.Add(LoopTraceHitResult);

		// If character is climbing make less iterations (rows)
		int InnerIndex = UParkourFunctionLibrary::SelectParkourStateFloat(29.0f, .0f, .0f, 6.0f, ParkourStateTag);

		// Loop that traces each row line for one column
		for (int32 k = 0; k <= InnerIndex; k++)
		{
			StartLocation.Z += 8.0f;
			EndLocation.Z += 8.0f;

			// Trace line that will indicate row in the column
			// Trace gets higher after each iteration
			PerformLineTraceByChannel(Character->GetWorld(), InnerLoopTraceHitResult, StartLocation, EndLocation, ECC_Visibility, bDrawDebug);

			// Record all traces for this column
			HopHitTraces.Add(InnerLoopTraceHitResult);
		}

		// Loop that looks through the entire column and adds only those that pass through trashhold 
		for (int32 j = 1; j < HopHitTraces.Num(); j++)
		{
			float Distance1 = HopHitTraces[j].bBlockingHit ? HopHitTraces[j].Distance : FVector::Distance(HopHitTraces[j].TraceStart, HopHitTraces[j].TraceEnd);
			float Distance2 = HopHitTraces[j - 1].bBlockingHit ? HopHitTraces[j - 1].Distance : FVector::Distance(HopHitTraces[j - 1].TraceStart, HopHitTraces[j - 1].TraceEnd);

			if (Distance1 - Distance2 > 5.0f)
			{
				WallHitTraces.Add(HopHitTraces[j - 1]);
				break;
			}
		}
	}

	// If no valid trace was recorded stop execution
	if (WallHitTraces.Num() <= 0)
		return;

	// Take first from the array
	WallHitResult = WallHitTraces[0];

	// Loop that determines closest to character line trace
	for (int32 i = 1; i < WallHitTraces.Num(); i++)
	{
		float Distance1 = FVector::Distance(Character->GetActorLocation(), WallHitTraces[i].ImpactPoint);
		float Distance2 = FVector::Distance(Character->GetActorLocation(), WallHitResult.ImpactPoint);
		if (Distance1 <= Distance2)
		{
			WallHitResult = WallHitTraces[i];
		}
	}

	// Check whether hit result is valid
	if (!WallHitResult.bBlockingHit || WallHitResult.bStartPenetrating)
		return;

	// If character is bit climbing set new wall rotation
	if (!ParkourStateTag.GetTagName().IsEqual("Parkour.State.Climb"))
	{
		WallRotation = UParkourFunctionLibrary::NormalReverseRotationZ(WallHitResult.ImpactNormal);
	}

	// Loop that checks where is wall's valid top point
	for (int32 i = 0; i <= 8; i++)
	{
		FVector ForwardVector = WallRotation.RotateVector(FVector::ForwardVector);

		StartLocation = (ForwardVector * (i * 30.0f + 2.0f)) + WallHitResult.ImpactPoint + FVector(0.0f, 0.0f, 7.0f);
		EndLocation = StartLocation - FVector(0.0f, 0.0f, 7.0f);

		// Trace sphere that pokes wall's top side 
		// Trace gets further away from character after each iteration
		PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 2.5f, ECC_Visibility, bDrawDebug);

		// If sphere trace hit something at first iteration then set result as wall top and top hit
		// If sphere trace hasn`t hit anything stop iterating (top side has ended or is uneven)
		if (i == 0 && HitResult.bBlockingHit)
		{
			WallTopResult = HitResult;
			TopHits = HitResult;
		}
		else if (HitResult.bBlockingHit)
		{
			TopHits = HitResult;
		}
		else
		{
			break;
		}
	}

	// Do not continue if character is doing something (like climbing, maunting, etc.)
	if (!ParkourStateTag.GetTagName().IsEqual("Parkour.State.NotBusy"))
		return;

	FVector ForwardVector = WallRotation.RotateVector(FVector::ForwardVector);

	StartLocation = TopHits.ImpactPoint + (ForwardVector * 30.0f);
	EndLocation = TopHits.ImpactPoint;

	// Trace sphere that will check whether wall's top side has any objects on top (like another wall)
	PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 2.5f, ECC_Visibility, bDrawDebug);

	if (!HitResult.bBlockingHit)
		return;

	WallDepthResult = HitResult;

	StartLocation = WallDepthResult.ImpactPoint + ForwardVector * 70.0f;
	EndLocation = StartLocation - FVector(0.0f, 0.0f, 200.0f);

	// Trace sphere that will check whether wall is thin or thick 
	PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 10.0f, ECC_Visibility, bDrawDebug);

	// If hit result valid set it as wall vault 
	if (HitResult.bBlockingHit)
	{
		WallVaultResult = HitResult;
	}
}

bool UParkourComponent::CheckMantleSurface()
{
	FVector StartLocation = WallTopResult.ImpactPoint + FVector(0.0f, 0.0f, CapsuleComponent->GetUnscaledCapsuleHalfHeight() + 8.0f);
	FVector EndLocation = StartLocation;
	float HalfHeight = CapsuleComponent->GetUnscaledCapsuleHalfHeight() - 8.0f;
	float Radius = 25.0f;

	// Trace capsule to check whether surface is valid for Mantle movement (capsule height is the same as character has)
	FHitResult HitResult;
	PerformCapsuleTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, HalfHeight, Radius, ECC_Visibility, bDrawDebug);

	return !HitResult.bBlockingHit;
}

bool UParkourComponent::CheckVaultSurface()
{
	FVector StartLocation = WallTopResult.ImpactPoint + FVector(0.0f, 0.0f, (CapsuleComponent->GetUnscaledCapsuleHalfHeight() / 2) + 18.0f);
	FVector EndLocation = StartLocation;
	float HalfHeight = (CapsuleComponent->GetUnscaledCapsuleHalfHeight() / 2) + 5.0f;
	float Radius = 25.0f;

	// Trace capsule to check whether surface is valid for Vault movement (capsule height is half size as character has)
	FHitResult HitResult;
	PerformCapsuleTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, HalfHeight, Radius, ECC_Visibility, bDrawDebug);

	return !HitResult.bBlockingHit;
}

bool UParkourComponent::CheckClimbSurface()
{
	FVector ForwardVector = WallRotation.RotateVector(FVector::ForwardVector);

	FVector StartLocation = WallTopResult.ImpactPoint + FVector(0.0f, 0.0f, -90.0f) + (ForwardVector * -55.0f);
	float HalfHeight = CapsuleComponent->GetUnscaledCapsuleHalfHeight();
	float Radius = 25.0f;

	// Trace capsule to check whether surface is valid for climbing it (capsule height is the same as character has)
	FHitResult HitResult;
	PerformCapsuleTraceByChannel(Character->GetWorld(), HitResult, StartLocation, StartLocation, HalfHeight, Radius, ECC_Visibility, bDrawDebug);

	return !HitResult.bBlockingHit;
}

void UParkourComponent::CheckClimbStyle(const FHitResult& HitResult, const FRotator& Rotation)
{
	FVector ForwardVector = Rotation.RotateVector(FVector::ForwardVector);

	FVector StartLocation = HitResult.ImpactPoint - ForwardVector * 10.0f;
	StartLocation.Z -= 125.0f;
	FVector EndLocation = StartLocation + ForwardVector * 30.0f;

	// Trace sphere to check whether under edge enough space to place feet
	FHitResult TraceResult;
	PerformSphereTraceByChannel(Character->GetWorld(), TraceResult, StartLocation, EndLocation, 10.0f, ECC_Visibility, bDrawDebug, 0.0f);

	// If something blocked trace than climb style can be "Braced"
	if (TraceResult.bBlockingHit)
		SetClimbStyle(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.ClimbStyle.Braced")));
	else
		SetClimbStyle(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.ClimbStyle.FreeHang")));
}

float UParkourComponent::FirstTraceHeight() const
{
	// If character is not climbing return preset value
	if (!ParkourStateTag.GetTagName().IsEqual("Parkour.State.Climb"))
		return -60.0f;

	float ClimbHeight = 0.0f;

	// Loop for setting up climb height calculated from hands position
	for (int32 i = 0; i < 5; i++)
	{


		FVector StartLocation = Character->GetActorLocation();
		float RightHandZLocation = CharacterMesh->GetSocketLocation("hand_r").Z;
		float LeftHandZLocation = CharacterMesh->GetSocketLocation("hand_l").Z;
		StartLocation.Z = RightHandZLocation < LeftHandZLocation ? LeftHandZLocation : RightHandZLocation;
		StartLocation.Z -= CharacterHeightDiff - CharacterHandUp;
		StartLocation -= Character->GetActorForwardVector() * 20.0f;

		FVector EndLocation = StartLocation + Character->GetActorForwardVector() * 20.0f * (i + 1);

		// Trace sphere that gets wall point on the hands level
		// End gets further away from character after each iteration
		FHitResult FirstHitResult;
		PerformSphereTraceByChannel(Character->GetWorld(), FirstHitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug);

		// Skip iteration if no wall was detected
		if (!FirstHitResult.bBlockingHit)
			continue;

		for (int32 j = 0; j < 10; j++)
		{
			FRotator ReversedRotator = UParkourFunctionLibrary::NormalReverseRotationZ(FirstHitResult.ImpactNormal);
			FVector ForwardVector = ReversedRotator.RotateVector(FVector::ForwardVector);
			StartLocation = FirstHitResult.ImpactPoint + ForwardVector * 2.0f;
			StartLocation.Z += 5.0f + j * 10.0f;

			EndLocation = StartLocation;
			EndLocation.Z -= 25.0f + j * -5.0f;

			// Trace sphere that gets wall top point
			// Trace gets higher after each iteration
			FHitResult SecondHitResult;
			PerformSphereTraceByChannel(Character->GetWorld(), SecondHitResult, StartLocation, EndLocation, 2.5f, ECC_Visibility, bDrawDebug);

			// If hit result is valid record it
			if (SecondHitResult.bBlockingHit && !SecondHitResult.bStartPenetrating)
			{
				ClimbHeight = SecondHitResult.ImpactPoint.Z;
				break;
			}
		}

		break;
	}

	// return height difference from character with small offset
	return ClimbHeight - Character->GetActorLocation().Z - 4.0f;
}

void UParkourComponent::CheckClimbOrHop()
{
	FName DesireRotationName = GetClimbDesireRotation().GetTagName();

	// If direction is not Forward use hop action
	if (DesireRotationName.IsEqual("Parkour.Direction.Forward") ||
		DesireRotationName.IsEqual("Parkour.Direction.ForwardLeft") ||
		DesireRotationName.IsEqual("Parkour.Direction.ForwardRight"))
	{
		// If surface hasn't passed mantle check use hop action
		if (!CheckMantleSurface())
		{
			HopAction();
			return;
		}

		if (ClimbStyle.GetTagName().IsEqual("Parkour.ClimbStyle.Braced"))
			SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.ClimbingUp")));
		else
			SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.FreeHangClimbUp")));
	}
	else
	{
		HopAction();
	}
}

bool UParkourComponent::CheckAirHang() const
{
	if (!SecondClimbLedgeResult.bBlockingHit)
		return false;

	float HeadLocationZ = CharacterMesh->GetSocketLocation("head").Z;
	float LedgeLocationZ = SecondClimbLedgeResult.ImpactPoint.Z;

	//Check whether  character on ground or diference of head location and ledge location higher than the threshold
	if (bOnGround || HeadLocationZ - LedgeLocationZ <= 30.0f)
		return false;

	return true;
}

void UParkourComponent::CheckDistance()
{
	if (WallHitResult.bBlockingHit)
	{
		// Set all values to 0
		WallHeight = .0f;
		WallDepth = .0f;
		VaultHeight = .0f;

		// Reset values from recorded hit results
		if (WallTopResult.bBlockingHit)
			WallHeight = WallTopResult.ImpactPoint.Z - CharacterMesh->GetSocketLocation("root").Z;
		if (WallTopResult.bBlockingHit && WallDepthResult.bBlockingHit)
			WallDepth = FVector::Distance(WallTopResult.ImpactPoint, WallDepthResult.ImpactPoint);
		if (WallDepthResult.bBlockingHit && WallVaultResult.bBlockingHit)
			VaultHeight = WallDepthResult.ImpactPoint.Z - WallVaultResult.ImpactPoint.Z;

		UE_LOG(LogTemp, Log, TEXT("WallHeight: %f, WallDepth: %f, VaultHeight: %f"), WallHeight, WallDepth, VaultHeight);
	}
}

bool UParkourComponent::CheckClimbMoveSurface(const FHitResult& MovementHitResult) const
{
	FRotator ArrowRotation = ArrowActor->GetArrowComponent()->GetComponentRotation();
	FVector RightVector = ArrowRotation.RotateVector(FVector::RightVector);
	FVector ForwardVector = ArrowRotation.RotateVector(FVector::ForwardVector);

	FVector StartLocation = MovementHitResult.ImpactPoint + GetHorizontalAxis() * RightVector * 13.0f - ForwardVector * 40.0f;
	StartLocation.Z -= 90.0f;

	FVector EndLocation = StartLocation + ForwardVector * 15.0f;

	// Trace Capsule that checks whether something alongside the wall
	FHitResult HitResult;
	PerformCapsuleTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, CapsuleComponent->GetUnscaledCapsuleHalfHeight(), 5.0f, ECC_Visibility, bDrawDebug, 0.0f);

	//If nothing blocks trace result then character can move while climbing
	return !HitResult.bBlockingHit;
}

bool UParkourComponent::CheckOutCorner(int32& OutCornerIndex) const
{
	bool OutCornerResult;

	// Set corner index to pass later
	OutCornerIndex = 0;

	// Loop to check whether wall has any corner
	for (int32 i = -2; i < 4; i++)
	{
		FVector RightVector = ArrowActor->GetArrowComponent()->GetComponentRotation().RotateVector(FVector::RightVector);
		FVector ForwardVector = ArrowActor->GetArrowComponent()->GetComponentRotation().RotateVector(FVector::ForwardVector);

		FVector StartLocation = ArrowActor->GetArrowComponent()->GetComponentLocation() + RightVector * (GetHorizontalAxis() * 35.0f);
		StartLocation.Z -= i * 10.0f; // Should be same as FOutCornerMoveParams.IndexMultiplierZOffset

		FVector EndLocation = StartLocation + ForwardVector * 100.0f;

		// Trace sphere that checks whether wall has an out corner
		// Trace gets lower after each iteration
		FHitResult HitResult;
		PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug, 0.0f);

		OutCornerResult = HitResult.bStartPenetrating;

		if (OutCornerResult)
		{
			// Set corner index to pass it back
			OutCornerIndex = i;
			break;
		}
	}

	return OutCornerResult;
}

bool UParkourComponent::CheckInCorner()
{
	FRotator ArrowRotation = ArrowActor->GetArrowComponent()->GetComponentRotation();
	FVector ArrowRightVector = ArrowRotation.RotateVector(FVector::RightVector);
	FVector ArrowForwardVector = ArrowRotation.RotateVector(FVector::ForwardVector);
	FVector ArrowLocation = ArrowActor->GetArrowComponent()->GetComponentLocation();

	FHitResult LocalCornerDepth;
	FHitResult LocalTopResult;
	FHitResult HitResult;

	// Loop to check whether wall ends  
	for (int32 i = 0; i < 6; i++)
	{
		FVector StartLocation = ArrowLocation + ArrowRightVector * GetHorizontalAxis() * i * 10.0f;
		StartLocation -= ArrowForwardVector * 10.0f;
		FVector EndLocation = StartLocation + ArrowForwardVector * 90.0f;

		// Trace sphere that checks whether wall has an out corner
		// Trace gets further from character to side after each iteration
		PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug, 0.0f);

		if (!HitResult.bBlockingHit)
			break;

		// If it is the last iteration, then stop movement to prevent visual bugs
		// If it is not, then record corner depth
		if (i == 5)
		{
			StopClimbMovement();
			return false;
		}
		else
			LocalCornerDepth = HitResult;
	}

	// Loop to check is the end of the wall is an actual corner by checking its side 
	for (int32 i = 0; i < 3; i++)
	{
		FVector StartLocation = LocalCornerDepth.ImpactPoint + ArrowForwardVector * 10.0f;
		StartLocation += ArrowRightVector * GetHorizontalAxis() * 20.0f;
		StartLocation.Z -= i * 10.0f;
		FVector EndLocation = StartLocation - ArrowRightVector * GetHorizontalAxis() * 70.0f;

		// Trace sphere that checks whether there is a wall from LocalCornerDepth point
		// Trace gets lower after each iteration
		PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug);

		if (HitResult.bBlockingHit)
			break;

		// If on the last iteration nothing blocked trace, then there is no valid corner
		if (i == 2)
		{
			StopClimbMovement();
			return false;
		}
	}

	// Loop to check where is corner's top side
	for (int32 i = 0; i < 5; i++)
	{
		FRotator ReverseRotation = UParkourFunctionLibrary::NormalReverseRotationZ(HitResult.ImpactNormal);
		FVector ForwardVector = ReverseRotation.RotateVector(FVector::ForwardVector);

		FVector StartLocation = HitResult.ImpactPoint + ForwardVector * 2.0f;
		StartLocation.Z += 5.0f * (i + 1);
		FVector EndLocation = StartLocation;
		EndLocation.Z -= 5.0f * (i + 10);

		// Trace sphere that pokes at top side of the wall at "corner" location
		// Start gets higher after each iteration
		// End gets lower after each iteration
		PerformSphereTraceByChannel(Character->GetWorld(), LocalTopResult, StartLocation, EndLocation, 2.5f, ECC_Visibility, bDrawDebug);

		//If trace wasn't blocked by anything than corner is not valid  
		if (!LocalTopResult.bBlockingHit)
		{
			StopClimbMovement();
			return false;
		}
	}

	FRotator ReverseRotation = UParkourFunctionLibrary::NormalReverseRotationZ(HitResult.ImpactNormal);
	FVector ForwardVector = ReverseRotation.RotateVector(FVector::ForwardVector);

	FVector StartLocation = LocalTopResult.ImpactPoint - ForwardVector * 50.0f;
	StartLocation.Z -= CapsuleComponent->GetUnscaledCapsuleHalfHeight();

	// Trace capsule that checks whether character will fit and won't collide with anything
	FHitResult CapsuleCheckResult;
	PerformCapsuleTraceByChannel(Character->GetWorld(), CapsuleCheckResult, StartLocation, StartLocation,
		CapsuleComponent->GetUnscaledCapsuleHalfHeight(), 27.0f, ECC_Visibility, bDrawDebug);

	// If something is blocking, then corner is not volid 
	if (CapsuleCheckResult.bBlockingHit)
	{
		StopClimbMovement();
		return false;
	}

	// Record wall rotation
	WallRotation = ReverseRotation;

	// Set location depending on style that character is climbing
	float StyleMultiplier = UParkourFunctionLibrary::SelectClimbStyleFloat(44.0f, 9.0f, ClimbStyle);
	FVector TargetRelativeLocation = HitResult.ImpactPoint - ForwardVector * StyleMultiplier;
	TargetRelativeLocation.Z = LocalTopResult.ImpactPoint.Z;
	TargetRelativeLocation.Z += CharacterHeightDiff - UParkourFunctionLibrary::SelectClimbStyleFloat(107.0f, 118.0f, ClimbStyle);

	CornerMove(TargetRelativeLocation, WallRotation);

	return true;
}

bool UParkourComponent::CheckInCornerHop()
{
	// Corner hop is not possible with Forward and Backward direction
	if (GetClimbDesireRotation().GetTagName().IsEqual("Parkour.Direction.Forward") ||
		GetClimbDesireRotation().GetTagName().IsEqual("Parkour.Direction.Backward"))
		return false;

	// Initialize all needed variables for further use
	FRotator ArrowRotation = ArrowActor->GetArrowComponent()->GetComponentRotation();
	FVector ArrowRightVector = ArrowRotation.RotateVector(FVector::RightVector);
	FVector ArrowForwardVector = ArrowRotation.RotateVector(FVector::ForwardVector);

	FHitResult LocalCornerDepth;

	// Loop to check whether wall ends
	for (int32 i = 0; i < 6; i++)
	{
		FVector StartLocation = ArrowActor->GetArrowComponent()->GetComponentLocation();
		StartLocation += ArrowRightVector * GetHorizontalAxis() * i * 10.0f;
		StartLocation -= ArrowForwardVector * 10.0f;
		FVector EndLocation = StartLocation + ArrowForwardVector * 90.0f;


		// Trace sphere that checks where wall has an end
		// Trace gets further from character to side after each iteration
		FHitResult HitResult;
		PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug);

		// If nothing blocks trace corner is valid for hopping
		// No need to check first one, because first iteration always hits something (character holding on something)
		if (!HitResult.bBlockingHit)
			break;
		else if (i != 5)
			LocalCornerDepth = HitResult;
		else
			return false;
	}

	// Loop that sets hor rotation
	for (int32 i = 0; i < 23; i++)
	{
		FVector StartLocation = LocalCornerDepth.ImpactPoint + ArrowForwardVector * 10.0f;
		StartLocation.Z = Character->GetActorLocation().Z + i * 8.0f;
		StartLocation += ArrowRightVector * GetHorizontalAxis() * 10.0f;
		FVector EndLocation = StartLocation - ArrowRightVector * GetHorizontalAxis() * 60.0f;

		// Trace sphere that pokes wall from other side from character center to top of the wall to get wall rotation
		// Trace gets higher after each iteration
		FHitResult HitResult;
		PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug);

		// If something blocked trace, corner is valid
		if (HitResult.bBlockingHit && !HitResult.bStartPenetrating)
		{
			CornerHopRotation = UParkourFunctionLibrary::NormalReverseRotationZ(HitResult.ImpactNormal);
			return true;
		}
	}

	return false;
}

bool UParkourComponent::CheckLedgeValid()
{
	// Check whether any traces were recorded
	if (WallHitTraces.Num() <= 0)
	{
		ResetParkourResults();
		return false;
	}
	else
		return true;
}

bool UParkourComponent::CheckOutCornerHop()
{
	// Initialize all needed variables for further use
	FRotator ArrowRotation = ArrowActor->GetArrowComponent()->GetComponentRotation();
	FVector ArrowRightVector = ArrowRotation.RotateVector(FVector::RightVector);
	FVector ArrowForwardVector = ArrowRotation.RotateVector(FVector::ForwardVector);

	// Loop to check whether on the side there is a wall to hop on
	for (int32 i = 0; i < 23; i++)
	{
		FVector StartLocation = ArrowActor->GetArrowComponent()->GetComponentLocation() - ArrowForwardVector * 20.0f;
		StartLocation.Z = Character->GetActorLocation().Z + i * 8.0f;
		StartLocation -= ArrowRightVector * GetHorizontalAxis() * 20.0f;
		FVector EndLocation = StartLocation + ArrowRightVector * GetHorizontalAxis() * 120.0f;

		// Trace sphere that checks whether there is a wall on the side
		// Trace gets higher after each iteration
		FHitResult HitResult;
		PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug);

		// If something blocked trace, corner is valid
		if (HitResult.bBlockingHit && !HitResult.bStartPenetrating)
		{
			CornerHopRotation = UParkourFunctionLibrary::NormalReverseRotationZ(HitResult.ImpactNormal);
			return true;
		}
	}

	return false;
}

void UParkourComponent::PerformSphereTraceByChannel(UWorld* World, FHitResult& HitResult, const FVector& StartLocation, const FVector& EndLocation,
	float Radius, ECollisionChannel TraceChannel, bool bDrawDebugSphere, float DrawTime /*= 2.0f*/) const
{
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("PerformSphereTraceByChannel: Invalid world reference."));
		return;
	}

	FCollisionQueryParams TraceParams(FName(TEXT("SphereTrace")));
	World->SweepSingleByChannel(HitResult, StartLocation, EndLocation, FQuat::Identity, TraceChannel, FCollisionShape::MakeSphere(Radius), TraceParams);

	if (bDrawDebugSphere)
	{

		// Draw sphere at the start of the trace and where it hits if it is
		if (HitResult.bBlockingHit)
			DrawDebugSphere(World, HitResult.Location, Radius, 32, FColor::Red, false, DrawTime);
		if (HitResult.bStartPenetrating)
			DrawDebugSphere(World, StartLocation, Radius, 8, FColor::Red, false, DrawTime);
		else
			DrawDebugSphere(World, StartLocation, Radius, 8, FColor::Green, false, DrawTime);
	}
}

void UParkourComponent::PerformBoxTraceByChannel(UWorld* World, FHitResult& HitResult, const FVector& StartLocation, const FVector& EndLocation,
	const FVector& BoxHalfExtend, ECollisionChannel CollisionChannel, bool bDrawDebugBox, float DrawTime /*= 2.0f*/) const
{
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("PerformBoxTraceByChannel: Invalid world reference."));
		return;
	}

	FCollisionQueryParams TraceParams(FName(TEXT("BoxTrace")));
	World->SweepSingleByChannel(HitResult, StartLocation, EndLocation, FQuat::Identity, CollisionChannel, FCollisionShape::MakeBox(BoxHalfExtend), TraceParams);

	if (bDrawDebugBox)
	{
		// Draw box at the start of the trace or where it hits if it is
		if (HitResult.bBlockingHit)
			DrawDebugBox(World, HitResult.Location, BoxHalfExtend, FColor::Red, false, DrawTime);
		else
			DrawDebugBox(World, StartLocation, BoxHalfExtend, FColor::Green, false, DrawTime);
	}
}

void UParkourComponent::PerformCapsuleTraceByChannel(UWorld* World, FHitResult& HitResult, const FVector& StartLocation, const FVector& EndLocation,
	float HalfHeight, float Radius, ECollisionChannel CollisionChannel, bool bDrawDebugCapsule, float DrawTime /*= 2.0f*/) const
{
	if (!true)
	{
		UE_LOG(LogTemp, Error, TEXT("PerformCapsuleTraceByChannel: Invalid world reference."));
		return;
	}

	FCollisionQueryParams TraceParams(FName(TEXT("CapsuleTrace")));
	World->SweepSingleByChannel(HitResult, StartLocation, EndLocation, FQuat::Identity, CollisionChannel, FCollisionShape::MakeCapsule(Radius, HalfHeight), TraceParams);

	if (bDrawDebugCapsule)
	{
		// Draw capsule at the start of the trace and where it hits if it is
		if (HitResult.bBlockingHit)
			DrawDebugCapsule(Character->GetWorld(), HitResult.Location, HalfHeight, Radius, FQuat::Identity, FColor::Red, false, DrawTime);
		if (HitResult.bStartPenetrating)
			DrawDebugCapsule(Character->GetWorld(), StartLocation, HalfHeight, Radius, FQuat::Identity, FColor::Red, false, DrawTime);
		else
			DrawDebugCapsule(Character->GetWorld(), StartLocation, HalfHeight, Radius, FQuat::Identity, FColor::Green, false, DrawTime);
	}
}

void UParkourComponent::PerformLineTraceByChannel(UWorld* World, FHitResult& HitResult, const FVector& StartLocation, const FVector& EndLocation,
	ECollisionChannel CollisionChannel, bool bDrawDebugLine, float DrawTime /*= 1.0f*/) const
{
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("PerformLineTraceByChannel: Invalid world reference."));
		return;
	}

	World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, CollisionChannel);

	if (bDrawDebugLine)
	{
		// Draw line at the start of the trace or if it hits something draw it with point
		if (HitResult.bBlockingHit)
		{
			DrawDebugLine(World, StartLocation, EndLocation, FColor::Red, false, DrawTime);
			DrawDebugPoint(World, HitResult.ImpactPoint, 10.0f, FColor::Red, false, DrawTime);
		}
		else
			DrawDebugLine(World, StartLocation, EndLocation, FColor::Green, false, DrawTime);
	}
}

void UParkourComponent::ChangeDebugMode()
{
	// Hide or show arrow and reset flag for drawing debug
	ArrowActor->GetArrowComponent()->SetHiddenInGame(bDrawDebug);
	bDrawDebug = !bDrawDebug;
}

void UParkourComponent::ResetParkourResults()
{
	HopHitTraces.Empty();
	WallHitTraces.Empty();
	WallHitResult = FHitResult();
	WallTopResult = FHitResult();
	WallDepthResult = FHitResult();
	WallVaultResult = FHitResult();
	TopHits = FHitResult();
	FirstClimbLedgeResult = FHitResult();
	SecondClimbLedgeResult = FHitResult();
}

void UParkourComponent::ShowHitResults()
{
	if (WallTopResult.bBlockingHit)
		DrawDebugSphere(Character->GetWorld(), WallTopResult.ImpactPoint, 5.0f,
			12, FColor::Blue, false, 1.0f);
	if (WallDepthResult.bBlockingHit && !WallDepthResult.bStartPenetrating)
		DrawDebugSphere(Character->GetWorld(), WallDepthResult.ImpactPoint, 5.0f,
			12, FColor::Cyan, false, 1.0f);
	if (WallVaultResult.bBlockingHit && !WallVaultResult.bStartPenetrating)
		DrawDebugSphere(Character->GetWorld(), WallVaultResult.ImpactPoint, 5.0f,
			12, FColor::Magenta, false, 1.0f);
	if (SecondClimbLedgeResult.bBlockingHit)
		DrawDebugSphere(Character->GetWorld(), SecondClimbLedgeResult.ImpactPoint, 5.0f,
			12, FColor::Black, false, 1.0f);
}

FRotator UParkourComponent::GetDesiredRotation() const
{
	// Ignore if there is no input
	if (ForwardScale == 0 && RightScale == 0)
		return Character->GetActorRotation();

	FRotator ControlRot = Character->GetControlRotation();
	ControlRot.Roll = 0.0f;
	ControlRot.Pitch = 0.0f;

	FVector ForwardVector = ControlRot.RotateVector(FVector::ForwardVector);
	FVector RightVector = ControlRot.RotateVector(FVector::RightVector);

	FVector DirectionVector = ForwardVector * ForwardScale + RightVector * RightScale;
	DirectionVector.Normalize();

	FMatrix RotationMatrix = FRotationMatrix::MakeFromX(DirectionVector);
	FRotator RotatorFromX = RotationMatrix.Rotator();
	RotatorFromX.Normalize();

	return RotatorFromX;
}

void UParkourComponent::FindDropDownHangLocation()
{
	FVector StartLocation = Character->GetActorLocation();
	FVector EndLocation = StartLocation - FVector(0.0f, 0.0f, 120.0f);

	// Trace sphere to check whether is something under character
	FHitResult HitResult;
	PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 35.0f, ECC_Visibility, bDrawDebug);

	if (!HitResult.bBlockingHit || HitResult.bStartPenetrating)
		return;

	FVector ForwardVector = GetDesiredRotation().RotateVector(FVector::ForwardVector);

	StartLocation = HitResult.ImpactPoint + ForwardVector * 100.0f;
	StartLocation.Z -= 5.0f;

	EndLocation = StartLocation - ForwardVector * 125.0f;

	// Trace sphere to check whether is something in the way from floor point to the looking direction
	PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug);

	if (!HitResult.bBlockingHit || HitResult.bStartPenetrating)
		return;

	WallHitTraces.Empty();
	FRotator ReverseRotator = UParkourFunctionLibrary::NormalReverseRotationZ(HitResult.ImpactNormal);
	FVector RightVector = ReverseRotator.RotateVector(FVector::RightVector);
	ForwardVector = ReverseRotator.RotateVector(FVector::ForwardVector);

	int32 Index = 4;

	// Loop to determine edge of the wall to drop down (this loop represents columns of line traces)
	for (int32 i = 0; i <= Index; i++)
	{
		// Right offset that depends on amount of iterations (columns)
		float Offset = (i * 2.0f - Index) * 10.0f;

		StartLocation = (RightVector * Offset) + HitResult.ImpactPoint - (ForwardVector * 40.0f);
		EndLocation = (RightVector * Offset) + HitResult.ImpactPoint + (ForwardVector * 30.0f);

		// Trace sphere to check whether is something in the way from floor point to the looking direction
		// Indicates first in the column
		FHitResult LoopTraceHitResult;
		PerformLineTraceByChannel(Character->GetWorld(), LoopTraceHitResult, StartLocation, EndLocation, ECC_Visibility, bDrawDebug);

		// Clean Hop traces for each iteration (column)
		HopHitTraces.Empty();

		// Add first line trace in column to take it into account as well
		HopHitTraces.Add(LoopTraceHitResult);

		// Loop that traces each row line for one column
		for (int32 j = 0; j < 12; j++)
		{
			// Trace line that will indicate row in the column
			// Trace gets higher after each iteration
			StartLocation.Z += 8.0f;
			EndLocation.Z += 8.0f;

			FHitResult InnerLoopTraceHitResult;
			PerformLineTraceByChannel(Character->GetWorld(), InnerLoopTraceHitResult, StartLocation, EndLocation, ECC_Visibility, bDrawDebug);

			// Record all traces for this column
			HopHitTraces.Add(InnerLoopTraceHitResult);
		}

		// Loop that looks through the entire column and adds only those that pass through trashhold 
		for (int32 j = 1; j < HopHitTraces.Num(); j++)
		{
			float Distance1 = HopHitTraces[j].bBlockingHit ? HopHitTraces[j].Distance : FVector::Distance(HopHitTraces[j].TraceStart, HopHitTraces[j].TraceEnd);
			float Distance2 = HopHitTraces[j - 1].bBlockingHit ? HopHitTraces[j - 1].Distance : FVector::Distance(HopHitTraces[j - 1].TraceStart, HopHitTraces[j - 1].TraceEnd);

			if (Distance1 - Distance2 > 5.0f)
			{
				WallHitTraces.Add(HopHitTraces[j - 1]);
				break;
			}
		}
	}

	// If no valid trace was recorded stop execution
	if (WallHitTraces.Num() <= 0)
		return;

	WallHitResult = WallHitTraces[0];

	// Loop that determines closest to character line trace
	for (int32 i = 1; i < WallHitTraces.Num(); i++)
	{
		float Distance1 = FVector::Distance(Character->GetActorLocation(), WallHitTraces[i].ImpactPoint);
		float Distance2 = FVector::Distance(Character->GetActorLocation(), WallHitResult.ImpactPoint);
		if (Distance1 <= Distance2)
		{
			WallHitResult = WallHitTraces[i];
		}
	}

	// Check whether hit result is valid
	if (!WallHitResult.bBlockingHit || WallHitResult.bStartPenetrating)
		return;

	WallRotation = UParkourFunctionLibrary::NormalReverseRotationZ(WallHitResult.ImpactNormal);
	ForwardVector = WallRotation.RotateVector(FVector::ForwardVector);

	StartLocation = WallHitResult.ImpactPoint + (ForwardVector * 2.0f);
	StartLocation.Z += 7.0f;
	EndLocation = StartLocation;
	EndLocation.Z -= 7.0f;

	//Trace sphere to get top of the wall
	PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 2.5f, ECC_Visibility, bDrawDebug);

	if (!HitResult.bBlockingHit)
		return;

	WallTopResult = HitResult;

	if (CheckClimbSurface())
	{
		CheckClimbStyle(WallTopResult, WallRotation);
		ClimbLedgeResultCalculation(SecondClimbLedgeResult);

		if (ClimbStyle.GetTagName().IsEqual("Parkour.ClimbStyle.Braced"))
			SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.DropDown")));
		else
			SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.FreeHangDropDown")));
	}
	else
		ResetParkourResults();
}

void UParkourComponent::FindHopLocation()
{
	// Set up distances to hop
	float VerticalDirectionMultiplier = UParkourFunctionLibrary::SelectDirectionFloat(1.0f, -7.5f, -2.5f, -2.5f, -1.0f, -4.0f, -1.0f, -4.0f, GetClimbDesireRotation());
	float HorizontalDirectionMultiplier = UParkourFunctionLibrary::SelectDirectionFloat(0.0f, 0.0f, -1.0f, 1.0f, -0.75f, -0.75f, 0.75f, 0.75f, GetClimbDesireRotation());
	VerticalHopDistance = 25.0f * VerticalDirectionMultiplier;
	HorizontalHopDistance = 140.0f * HorizontalDirectionMultiplier;

	WallHitTraces.Empty();

	// Loop to determine edge of the wall to hop (this loop represents columns of line traces)
	for (int32 i = 0; i < 7; i++)
	{
		FVector CharacterUpVector = Character->GetActorRotation().RotateVector(FVector::UpVector);
		FVector CharacterRightVector = Character->GetActorRotation().RotateVector(FVector::RightVector);

		FVector WallRightVector = WallRotation.RotateVector(FVector::RightVector);
		FVector WallForwardVector = WallRotation.RotateVector(FVector::ForwardVector);

		FVector StartLocation = WallTopResult.ImpactPoint;
		StartLocation += CharacterUpVector * VerticalHopDistance;
		StartLocation += CharacterRightVector * HorizontalHopDistance;
		StartLocation += WallRightVector * 20.0f * (i - 3);
		StartLocation -= WallForwardVector * 60.0f;

		FVector EndLocation = StartLocation + WallForwardVector * 120.0f;

		// Trace sphere to check whether there is some wall 
		// Indicates first in the column
		FHitResult OuterLoopTraceResult;
		PerformLineTraceByChannel(Character->GetWorld(), OuterLoopTraceResult, StartLocation, EndLocation, ECC_Visibility, bDrawDebug);

		HopHitTraces.Empty();

		// Make an offset a bit lower
		StartLocation.Z -= 16.0f;
		EndLocation.Z -= 16.0f;

		for (int32 k = 0; k <= 20; k++)
		{
			StartLocation.Z += 8.0f;
			EndLocation.Z += 8.0f;

			// Trace line that will indicate row in the column
			// Trace gets higher after each iteration
			FHitResult InnerLoopTraceResult;
			PerformLineTraceByChannel(Character->GetWorld(), InnerLoopTraceResult, StartLocation, EndLocation, ECC_Visibility, bDrawDebug);

			if (InnerLoopTraceResult.bStartPenetrating)
				continue;

			HopHitTraces.Add(InnerLoopTraceResult);
		}


		// Loop that looks through the entire column and adds only those that pass through trashhold and passes capsule check
		for (int32 j = 1; j < HopHitTraces.Num(); j++)
		{
			float Distance1 = HopHitTraces[j].bBlockingHit ? HopHitTraces[j].Distance : FVector::Distance(HopHitTraces[j].TraceStart, HopHitTraces[j].TraceEnd);
			float Distance2 = HopHitTraces[j - 1].bBlockingHit ? HopHitTraces[j - 1].Distance : FVector::Distance(HopHitTraces[j - 1].TraceStart, HopHitTraces[j - 1].TraceEnd);

			if (Distance1 - Distance2 <= 5.0f)
				continue;

			FRotator ReverseRotation = UParkourFunctionLibrary::NormalReverseRotationZ(HopHitTraces[j - 1].ImpactNormal);
			FVector ForwardVector = ReverseRotation.RotateVector(FVector::ForwardVector);
			FVector RightVector = ReverseRotation.RotateVector(FVector::RightVector);

			float CapsuleHalfHeight = CapsuleComponent->GetUnscaledCapsuleHalfHeight() - 20.0f;

			FVector CapsuleStartLocation = HopHitTraces[j - 1].ImpactPoint - ForwardVector * 40.0f + RightVector * 4.0f;
			CapsuleStartLocation.Z -= CapsuleHalfHeight;

			FVector CapsuleEndLocation = CapsuleStartLocation - RightVector * 8.0f;

			// Trace capsule to check whether character will fit and won't collide with anything
			FHitResult CapsuleCheckTrace;
			PerformCapsuleTraceByChannel(Character->GetWorld(), CapsuleCheckTrace, CapsuleStartLocation, CapsuleEndLocation, CapsuleHalfHeight, 25.0f, ECC_Visibility, bDrawDebug);

			// If nothing blocks capsule trace than line trace can be considered
			if (!CapsuleCheckTrace.bBlockingHit)
				WallHitTraces.Add(HopHitTraces[j - 1]);

			break;

		}
	}

	FString DesireDirectionName = GetClimbDesireRotation().GetTagName().ToString();


	// If none of the traces were recorded than there could be a corner
	if (WallHitTraces.Num() <= 0)
	{
		if (DesireDirectionName.Equals("Parkour.Direction.Backward"))
			ParkourDrop();
		else if (bool bIsOutCorner = CheckOutCornerHop() || CheckInCornerHop())
		{
			WallRotation = CornerHopRotation;
			CornerHop(bIsOutCorner);
		}

		return;
	}

	WallHitResult = WallHitTraces[0];

	// Get Desired direction vector to get where to hop should be a priority
	FVector DesiredDirectionVector = FVector::ZeroVector;

	if (DesireDirectionName.Contains("Right"))
	{
		DesiredDirectionVector += Character->GetActorRightVector();
	}
	else if (DesireDirectionName.Contains("Left"))
	{
		DesiredDirectionVector += -Character->GetActorRightVector();
	}

	if (DesireDirectionName.Contains("Forward"))
	{
		DesiredDirectionVector += Character->GetActorUpVector();
	}
	else if (DesireDirectionName.Contains("Backward"))
	{
		DesiredDirectionVector += -Character->GetActorUpVector();
	}

	DesiredDirectionVector = DesiredDirectionVector.GetSafeNormal();

	// Variables to keep track of the best trace
	float BestAlignmentScore = -1.0f;
	float BestDistance = -1.0f;

	for (const FHitResult& HitResult : WallHitTraces)
	{
		FVector ToHitPoint = HitResult.ImpactPoint - Character->GetActorLocation();
		float Distance = ToHitPoint.Size();

		// Compute the alignment score based on the dot product
		FVector NormalizedToHitPoint = ToHitPoint.GetSafeNormal();
		float AlignmentScore = FVector::DotProduct(NormalizedToHitPoint, DesiredDirectionVector);

		// Determine if this trace is better firstдy by alignment and then by distance
		if (AlignmentScore > BestAlignmentScore || (AlignmentScore == BestAlignmentScore && Distance > BestDistance))
		{
			WallHitResult = HitResult;
			BestAlignmentScore = AlignmentScore;
			BestDistance = Distance;
		}
	}

	// Check whether hit result is valid
	if (WallHitResult.bStartPenetrating)
		return;

	// Set rotation if character is not climbing
	if (!ParkourStateTag.GetTagName().IsEqual("Parkour.State.Climb"))
		WallRotation = UParkourFunctionLibrary::NormalReverseRotationZ(WallHitResult.ImpactNormal);

	FVector StartLocation = WallHitResult.ImpactPoint;
	StartLocation.Z += 3.0f;
	FVector EndLocation = StartLocation;
	EndLocation.Z -= 3.0f;

	// Trace sphere to get top of the edge 
	FHitResult HitResult;
	PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug);

	if (!HitResult.bBlockingHit)
		return;

	WallTopResult = HitResult;
}

void UParkourComponent::GetClimbForwardValue(float ScaleValue, float& HorizontalForwardValue, float& VerticalForwardValue) const
{
	FRotator DeltaRotation = Character->GetControlRotation() - Character->GetActorRotation();
	DeltaRotation.Normalize();
	HorizontalForwardValue = ScaleValue * UKismetMathLibrary::DegSin(DeltaRotation.Yaw);
	VerticalForwardValue = ScaleValue * UKismetMathLibrary::DegCos(DeltaRotation.Yaw);
}

void UParkourComponent::GetClimbRightValue(float ScaleValue, float& HorizontalRightValue, float& VerticalRightValue) const
{
	FRotator DeltaRotation = Character->GetControlRotation() - Character->GetActorRotation();
	DeltaRotation.Normalize();
	HorizontalRightValue = ScaleValue * UKismetMathLibrary::DegCos(0.0f - DeltaRotation.Yaw);
	VerticalRightValue = ScaleValue * UKismetMathLibrary::DegSin(0.0f - DeltaRotation.Yaw);
}

float UParkourComponent::GetVerticalAxis() const
{
	if (ForwardScale == 0 && RightScale == 0)
		return 0.0f;

	return FMath::Clamp(VerticalClimbForwardValue + VerticalClimbRightValue, -1.0f, 1.0f);
}

float UParkourComponent::GetHorizontalAxis() const
{
	if (ForwardScale == 0 && RightScale == 0)
		return 0.0f;

	return FMath::Clamp(HorizontalClimbForwardValue + HorizontalClimbRightValue, -1.0f, 1.0f);
}

FGameplayTag UParkourComponent::GetClimbDesireRotation()
{
	// Here is simple table to show which values do what:
	// 
	// |                    |Z ∈ [0.5, 1.0] (Forward)	| Z ∈ [-0.5, 0.5] (Neutral)	| Z ∈ [-1.0, -0.5] (Backward)	|
	// |--------------------|---------------------------|---------------------------|-------------------------------|
	// |  Y ∈ [0.5, 1.0]	| Forward Right				| Right					    | Backward Right				|
	// |  Y ∈ [-0.5, 0.5]	| Forward					| Default (forward)			| Backward						|
	// |  Y ∈ [-1.0, -0.5]	| Forward Left				| Left					    | Backward Left					|


	float DesireRotationZ = GetVerticalAxis();
	float DesireRotationY = GetHorizontalAxis();

	if (DesireRotationZ >= 0.5f && DesireRotationZ <= 1.0f)
	{
		if (DesireRotationY >= 0.5f && DesireRotationY <= 1.0f)
			return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.ForwardRight"));
		if (DesireRotationY >= -0.5f && DesireRotationY <= 0.5f)
			return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.Forward"));
		if (DesireRotationY >= -1.0f && DesireRotationY <= -0.5f)
			return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.ForwardLeft"));
	}

	if (DesireRotationZ >= -0.5f && DesireRotationZ <= 0.5f)
	{
		if (DesireRotationY >= 0.5f && DesireRotationY <= 1.0f)
			return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.Right"));
		if (DesireRotationY >= -1.0f && DesireRotationY <= -0.5f)
			return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.Left"));
	}

	if (DesireRotationZ >= -1.0f && DesireRotationZ <= -0.5f)
	{
		if (DesireRotationY >= 0.5f && DesireRotationY <= 1.0f)
			return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.BackwardRight"));
		if (DesireRotationY >= -0.5f && DesireRotationY <= 0.5f)
			return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.Backward"));
		if (DesireRotationY >= -1.0f && DesireRotationY <= -0.5f)
			return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.BackwardLeft"));
	}

	return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.Forward"));
}

float UParkourComponent::GetClimbMoveSpeed() const
{
	// Get speed from the curve in the animation
	float BracedClampedSpeed = FMath::Clamp(AnimInstance->GetCurveValue("Climb Move Speed"), 1.0f, 98.0f);
	float FreeHangClampedSpeed = FMath::Clamp(AnimInstance->GetCurveValue("Climb Move Speed"), 1.0f, 55.0f);

	return UParkourFunctionLibrary::SelectClimbStyleFloat(BracedClampedSpeed * 0.1f, FreeHangClampedSpeed * 0.08f, ClimbStyle);
}

float UParkourComponent::GetClimbLeftHandZOffset() const
{
	if (ClimbDirection.GetTagName().IsEqual("Parkour.Direction.NoDirection"))
		return 0.0f;

	float BracedDirectionSubtract = ClimbDirection.GetTagName().IsEqual("Parkour.Direction.Right") ? 255.72f : 195.72f;
	float FreeHangDirectionSubtract = ClimbDirection.GetTagName().IsEqual("Parkour.Direction.Right") ? 229.09f : 227.6f;

	// Get left hand Z location from the curve in the animation
	float OriginalOffset = AnimInstance->GetCurveValue("Hand_L Z") - UParkourFunctionLibrary::SelectClimbStyleFloat(BracedDirectionSubtract, FreeHangDirectionSubtract, ClimbStyle);

	return FMath::Clamp(OriginalOffset, 0.0f, 5.0f);
}

float UParkourComponent::GetClimbRightHandZOffset() const
{
	if (ClimbDirection.GetTagName().IsEqual("Parkour.Direction.NoDirection"))
		return 0.0f;

	float BracedDirectionSubtract = ClimbDirection.GetTagName().IsEqual("Parkour.Direction.Right") ? 254.37f : 194.36f;
	float FreeHangDirectionSubtract = ClimbDirection.GetTagName().IsEqual("Parkour.Direction.Right") ? 227.6f : 229.09f;

	// Get right hand Z location from the curve in the animation
	float OriginalOffset = AnimInstance->GetCurveValue("Hand_R Z") - UParkourFunctionLibrary::SelectClimbStyleFloat(BracedDirectionSubtract, FreeHangDirectionSubtract, ClimbStyle);

	return FMath::Clamp(OriginalOffset, 0.0f, 5.0f);
}

void UParkourComponent::ParkourType(bool bAutoClimb)
{
	// If top of the wall is not found make character jump
	if (!WallTopResult.bBlockingHit)
	{
		SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.NoAction")));
		if (!bAutoClimb)
		{
			Character->Jump();
		}
		return;
	}

	// If character is climbing check on hop action, but if character doing something else stop execution
	if (!ParkourStateTag.GetTagName().IsEqual("Parkour.State.NotBusy"))
	{
		if (ParkourStateTag.GetTagName().IsEqual("Parkour.State.Climb"))
			CheckClimbOrHop();

		return;
	}

	// If character not on the ground check air hang action (if check wasn't passed execute climbing on the wall) 
	if (!bOnGround)
	{
		CheckAirHangOrClimb();
		return;
	}

	// If wall height is smaller than LowMantle threshold do nothing
	if (WallHeight > 0 && WallHeight <= 44)
	{
		SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.NoAction")));
		return;
	}

	// If wall height is bigger than LowMantle threshold and smaller then Mantle threshold execute LowMantle
	if (WallHeight > 44 && WallHeight < 90)
	{
		if (CheckMantleSurface())
			SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.LowMantle")));
		else
			SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.NoAction")));

		return;
	}

	// If wall height, vault height and wall depth meet all min and max requirements, then execute Vault
	if (WallHeight > 90 && WallHeight <= 160 && VaultHeight <= 160 && WallDepth >= 0 && WallDepth <= 120)
	{
		if (WallHeight <= 120 && VaultHeight <= 120 && WallDepth <= 30)
		{
			if (CheckVaultSurface())
				SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.ThinVault")));
			else
				SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.NoAction")));

			return;
		}

		if (WallDepth >= 0 && WallDepth <= 120 && CharacterMovement->Velocity.Length() > 20)
		{
			if (WallHeight <= 120 && VaultHeight <= 120)
			{
				if (CheckVaultSurface())
					SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.Vault")));
				else
					SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.NoAction")));

				return;
			}

			if (WallHeight > 120 && WallHeight <= 160 && VaultHeight <= 160)
			{
				if (CheckVaultSurface())
					SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.HighVault")));
				else
					SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.NoAction")));

				return;
			}
		}
	}

	// If wall height is bigger than Mantle threshold and smaller than maximum wall height execute Mantle 
	// (also means that wall height, vault height and wall depth are not volid because of previous check)
	if (WallHeight > 90 && WallHeight <= 160)
		if (CheckMantleSurface())
			SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.Mantle")));
		else
			SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.NoAction")));

	// If wall height bigger than Climb threshold execute climb action
	if (WallHeight < 250)
	{
		CheckAirHangOrClimb();
	}
}

void UParkourComponent::CheckAirHangOrClimb()
{
	if (!CheckClimbSurface())
		return;

	CheckClimbStyle(WallTopResult, WallRotation);
	ClimbLedgeResultCalculation(SecondClimbLedgeResult);
	if (CheckAirHang())
	{
		if (ClimbStyle.GetTagName().IsEqual("Parkour.ClimbStyle.Braced"))
			SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.FallingBraced")));
		else
			SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.FallingFreeHang")));
	}
	else
	{
		if (ClimbStyle.GetTagName().IsEqual("Parkour.ClimbStyle.Braced"))
			SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.Climb")));
		else
			SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.FreeHangClimb")));
	}
}

void UParkourComponent::SetParkourAction(const FGameplayTag& NewParkourAction)
{
	// If new parkour tag is  the same as current, then stop execution
	if (ParkourActionTag == NewParkourAction)
		return;

	ParkourActionTag = NewParkourAction;

	if (!AnimInstance->GetClass()->ImplementsInterface(UParkourABPInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetParkourAction: AnimInstance does not implement the ABP interface"));
		return;
	}

	IParkourABPInterface* ParkourABPInterface = Cast<IParkourABPInterface>(AnimInstance);
	ParkourABPInterface->Execute_SetParkourAction(AnimInstance, ParkourActionTag);

	if (!WidgetActor->WidgetComponent->GetWidget()->GetClass()->ImplementsInterface(UParkourStatsInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetParkourAction: Widget does not implement the Stats interface"));
		return;
	}

	IParkourStatsInterface* ParkourStatsInterface = Cast<IParkourStatsInterface>(WidgetActor->WidgetComponent->GetWidget());
	ParkourStatsInterface->Execute_SetParkourAction(WidgetActor->WidgetComponent->GetWidget(), ParkourActionTag.GetTagName().ToString());

	if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.ThinVault"))
	{
		ParkourVariables = ParkourVariablesCollection.ThinVaultDT;
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.HighVault"))
	{
		ParkourVariables = ParkourVariablesCollection.HighVaultDT;
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.Vault"))
	{
		ParkourVariables = ParkourVariablesCollection.VaultDT;
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.Mantle"))
	{
		ParkourVariables = ParkourVariablesCollection.MantleDT;
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.LowMantle"))
	{
		ParkourVariables = ParkourVariablesCollection.LowMantleDT;
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.Climb"))
	{
		ParkourVariables = ParkourVariablesCollection.BracedClimbDT;
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.FreeHangClimb"))
	{
		ParkourVariables = ParkourVariablesCollection.FreeHangDT;
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.ClimbingUp"))
	{
		ParkourVariables = ParkourVariablesCollection.BracedClimbUpDT;
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.FreeHangClimbUp"))
	{
		ParkourVariables = ParkourVariablesCollection.FreeHangClimbUpDT;
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.FallingBraced"))
	{
		ParkourVariables = ParkourVariablesCollection.FallingBracedClimbDT;
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.FallingFreeHang"))
	{
		ParkourVariables = ParkourVariablesCollection.FallingFreeHangClimbDT;
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.DropDown"))
	{
		ParkourVariables = ParkourVariablesCollection.BracedDropDownDT;
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.FreeHangDropDown"))
	{
		ParkourVariables = ParkourVariablesCollection.FreeHangDropDownDT;
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.ClimbHopUp"))
	{
		ParkourVariables = ParkourVariablesCollection.ClimbHopUpDT;
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.ClimbHopLeft"))
	{
		ParkourVariables = ParkourVariablesCollection.ClimbHopLeftDT;
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.ClimbHopRight"))
	{
		ParkourVariables = ParkourVariablesCollection.ClimbHopRightDT;
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.ClimbHopLeftUp"))
	{
		ParkourVariables = ParkourVariablesCollection.ClimbHopLeftUpDT;
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.ClimbHopRightUp"))
	{
		ParkourVariables = ParkourVariablesCollection.ClimbHopRightUpDT;
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.ClimbHopDown"))
	{
		ParkourVariables = ParkourVariablesCollection.ClimbHopDownDT;
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.FreeClimbHopLeft"))
	{
		ParkourVariables = ParkourVariablesCollection.FreeClimbHopLeftDT;
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.FreeClimbHopRight"))
	{
		ParkourVariables = ParkourVariablesCollection.FreeClimbHopRightDT;
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.FreeClimbHopDown"))
	{
		ParkourVariables = ParkourVariablesCollection.FreeClimbHopDownDT;
	}
	else
	{
		ResetParkourResults();
		return;
	}

	PlayParkourMontage();
}

void UParkourComponent::SetParkourState(const FGameplayTag& NewParkourState)
{
	// If new parkour tag is  the same as current, then stop execution
	if (ParkourStateTag == NewParkourState)
		return;

	PreviousStateSettings(ParkourStateTag, NewParkourState);

	ParkourStateTag = NewParkourState;

	if (!AnimInstance->GetClass()->ImplementsInterface(UParkourABPInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetParkourState: AnimInstance does not implement the ABP interface"));
		return;
	}

	IParkourABPInterface* ParkourABPInterface = Cast<IParkourABPInterface>(AnimInstance);
	ParkourABPInterface->Execute_SetParkourState(AnimInstance, ParkourStateTag);

	if (!WidgetActor->WidgetComponent->GetWidget()->GetClass()->ImplementsInterface(UParkourStatsInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetParkourState: Widget does not implement the Stats interface"));
		return;
	}

	IParkourStatsInterface* ParkourStatsInterface = Cast<IParkourStatsInterface>(WidgetActor->WidgetComponent->GetWidget());
	ParkourStatsInterface->Execute_SetParkourState(WidgetActor->WidgetComponent->GetWidget(), ParkourStateTag.GetTagName().ToString());

	FindMontageStartTime();

	if (ParkourStateTag.GetTagName().IsEqual("Parkour.State.Climb"))
	{
		SetUpParkourSettings(ECollisionEnabled::NoCollision, EMovementMode::MOVE_Flying, FRotator(.0f, 0.0f, .0f), true, true);
	}
	else if (ParkourStateTag.GetTagName().IsEqual("Parkour.State.Mantle"))
	{
		SetUpParkourSettings(ECollisionEnabled::NoCollision, EMovementMode::MOVE_Flying, FRotator(.0f, 500.0f, .0f), true, false);
	}
	else if (ParkourStateTag.GetTagName().IsEqual("Parkour.State.Vault"))
	{
		SetUpParkourSettings(ECollisionEnabled::NoCollision, EMovementMode::MOVE_Flying, FRotator(.0f, 500.0f, .0f), true, false);
	}
	else if (ParkourStateTag.GetTagName().IsEqual("Parkour.State.ReachLedge"))
	{
		SetUpParkourSettings(ECollisionEnabled::NoCollision, EMovementMode::MOVE_Flying, FRotator(.0f, 500.0f, .0f), true, false);
	}
	else if (ParkourStateTag.GetTagName().IsEqual("Parkour.State.NotBusy"))
	{
		SetUpParkourSettings(ECollisionEnabled::QueryAndPhysics, EMovementMode::MOVE_Walking, FRotator(.0f, 500.0f, .0f), true, false);
	}
}

void UParkourComponent::SetClimbStyle(const FGameplayTag& NewClimbStyle)
{
	// If new parkour tag is  the same as current, then stop execution
	if (ClimbStyle == NewClimbStyle)
		return;

	ClimbStyle = NewClimbStyle;

	if (!AnimInstance->GetClass()->ImplementsInterface(UParkourABPInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetClimbStyle: AnimInstance does not implement the ABP interface"));
		return;
	}

	IParkourABPInterface* ParkourABPInterface = Cast<IParkourABPInterface>(AnimInstance);
	ParkourABPInterface->Execute_SetClimbStyle(AnimInstance, ClimbStyle);

	if (!WidgetActor->WidgetComponent->GetWidget()->GetClass()->ImplementsInterface(UParkourStatsInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetClimbStyle: Widget does not implement the Stats interface"));
		return;
	}

	IParkourStatsInterface* ParkourStatsInterface = Cast<IParkourStatsInterface>(WidgetActor->WidgetComponent->GetWidget());
	ParkourStatsInterface->Execute_SetClimbStyle(WidgetActor->WidgetComponent->GetWidget(), ClimbStyle.GetTagName().ToString());
}

void UParkourComponent::SetClimbDirection(const FGameplayTag& NewClimbDirection)
{
	// If new direction tag is  the same as current, then stop execution
	if (ClimbDirection == NewClimbDirection)
		return;

	ClimbDirection = NewClimbDirection;

	if (!AnimInstance->GetClass()->ImplementsInterface(UParkourABPInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetClimbDirection: AnimInstance does not implement the ABP interface"));
		return;
	}

	IParkourABPInterface* ParkourABPInterface = Cast<IParkourABPInterface>(AnimInstance);
	ParkourABPInterface->Execute_SetClimbMovement(AnimInstance, ClimbDirection);
}

void UParkourComponent::SetUpParkourSettings(ECollisionEnabled::Type CollsionType, EMovementMode MovementMode, FRotator RotationRate, bool bDoCollisionTest, bool bStopImmediately)
{
	CapsuleComponent->SetCollisionEnabled(CollsionType);
	CharacterMovement->SetMovementMode(MovementMode);
	CharacterMovement->RotationRate = RotationRate;
	CameraBoom->bDoCollisionTest = bDoCollisionTest;

	if (bStopImmediately)
		CharacterMovement->StopMovementImmediately();
}

FGameplayTag UParkourComponent::SelectHopAction()
{
	FGameplayTag ForwardHopMovementTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.ClimbHopUp"));

	if (ClimbStyle.GetTagName().IsEqual("Parkour.ClimbStyle.Braced"))
	{
		// If character is doing corner hop, replace default forward with left or right
		if (CheckInCornerHop() || CheckOutCornerHop())
			ForwardHopMovementTag = GetHorizontalAxis() < 0.0f
			? UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.ClimbHopLeft"))
			: UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.ClimbHopRight"));


		return UParkourFunctionLibrary::SelectDirectionHopAction(
			ForwardHopMovementTag,
			UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.ClimbHopDown")),
			UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.ClimbHopLeft")),
			UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.ClimbHopRight")),
			UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.ClimbHopLeftUp")),
			UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.ClimbHopLeft")),
			UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.ClimbHopRightUp")),
			UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.ClimbHopRight")),
			GetHopDirection());
	}
	else
	{
		// If character is doing corner hop, replace default forward with left or right
		if (CheckInCornerHop() || CheckOutCornerHop())
			ForwardHopMovementTag = GetHorizontalAxis() < 0.0f
			? UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.FreeClimbHopLeft"))
			: UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.FreeClimbHopRight"));


		return UParkourFunctionLibrary::SelectDirectionHopAction(
			ForwardHopMovementTag,
			UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.FreeClimbHopDown")),
			UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.FreeClimbHopLeft")),
			UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.FreeClimbHopRight")),
			UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.FreeClimbHopLeft")),
			UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.FreeClimbHopLeft")),
			UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.FreeClimbHopRight")),
			UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.FreeClimbHopRight")),
			GetHopDirection());
	}

}

FGameplayTag UParkourComponent::GetHopDirection() const
{
	// FirstClimbLedgeResult is start point and SecondClimbLedgeResult is end point 
	FVector HopStartLocation = FirstClimbLedgeResult.ImpactPoint;
	FVector HopEndLocation = SecondClimbLedgeResult.ImpactPoint;

	FRotator LookAtRotation = UKismetMathLibrary::FindRelativeLookAtRotation(Character->GetTransform(), HopEndLocation);

	bool bIsForward = (HopEndLocation.Z - HopStartLocation.Z) > 37.0f;
	bool bIsBackward = (HopEndLocation.Z - HopStartLocation.Z) < -37.0f;

	bool bIsLeft = LookAtRotation.Yaw >= -135.0f && LookAtRotation.Yaw <= -35.0f;
	bool bIsRight = LookAtRotation.Yaw >= 35.0f && LookAtRotation.Yaw <= 135.0f;

	if (bIsForward)
	{
		if (bIsLeft)
			return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.ForwardLeft"));
		else if (bIsRight)
			return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.ForwardRight"));
		else
			return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.Forward"));
	}
	else if (bIsBackward)
	{
		if (bIsLeft)
			return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.BackwardLeft"));
		else if (bIsRight)
			return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.BackwardRight"));
		else
			return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.Backward"));
	}
	else if (bIsLeft)
		return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.Left"));
	else if (bIsRight)
		return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.Right"));
	else
		return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.Forward"));
}

void UParkourComponent::PreviousStateSettings(const FGameplayTag& PreviousState, const FGameplayTag& NewState)
{
	// Set Camera settings accordingly to next state
	if (PreviousState.GetTagName().IsEqual("Parkour.State.Climb"))
	{
		if (NewState.GetTagName().IsEqual("Parkour.State.ReachLedge"))
		{
			// Do nothing
		}
		else if (NewState.GetTagName().IsEqual("Parkour.State.Mantle"))
		{
			TargetRelativeCameraLocation = FirstTargetRelativeLocation;
			TargetArmLength = FirstCameraTargetArmLenght;
			AddCameraTimeline(0.4f);
		}
		else if (NewState.GetTagName().IsEqual("Parkour.State.NotBusy"))
		{
			TargetRelativeCameraLocation = FirstTargetRelativeLocation;
			TargetArmLength = FirstCameraTargetArmLenght;
			AddCameraTimeline(0.4f);
		}
	}
	else if (PreviousState.GetTagName().IsEqual("Parkour.State.NotBusy"))
	{
		if (NewState.GetTagName().IsEqual("Parkour.State.ReachLedge"))
		{
			TargetRelativeCameraLocation = FVector(-50.0f, 0.0f, 70.0f);
			TargetArmLength = 500.0f;
			AddCameraTimeline(0.4f);
		}
	}
}

void UParkourComponent::AddCameraTimeline(float Time)
{
	float WorldDeltaSeconds = GetWorld()->GetDeltaSeconds();
	Time += WorldDeltaSeconds;

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	// Set function on timeline finished to reset timer handles
	TimerManager.SetTimer(TimerHandle_FinishCameraTimeline, this, &UParkourComponent::FinishTimeline, 0.4, false);

	// Set function on timeline tick to change its location
	TimerManager.SetTimer(TimerHandle_TickCameraTimeline, this, &UParkourComponent::CameraTimelineTick, WorldDeltaSeconds, true);

}

void UParkourComponent::CameraTimelineTick()
{
	// Get Curve alpha from time that elapsed in timer
	float InTime = GetWorld()->GetTimerManager().GetTimerElapsed(TimerHandle_FinishCameraTimeline);
	CameraCurveAlpha = CameraCurve->GetFloatValue(InTime);

	// Get difference between target and current locations in relative type
	FVector LocationDiff = TargetRelativeCameraLocation - CameraBoom->GetRelativeLocation();

	// Set Relative location for camera
	CameraBoom->SetRelativeLocation(CameraBoom->GetRelativeLocation() + (LocationDiff * CameraCurveAlpha));

	// Calculate and set arm lenght
	float TargetArmLenghtDiff = TargetArmLength - CameraBoom->TargetArmLength;
	CameraBoom->TargetArmLength += TargetArmLenghtDiff * CameraCurveAlpha;
}

void UParkourComponent::FinishTimeline()
{
	// Clean up timer handles to avoid timer related problems
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_FinishCameraTimeline);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_TickCameraTimeline);
	CameraCurveAlpha = 0.0f;
}

void UParkourComponent::ClimbLedgeResultCalculation(FHitResult& ClimbLedgeResult)
{
	FRotator ReverseRotation = UParkourFunctionLibrary::NormalReverseRotationZ(WallHitResult.ImpactNormal);
	FVector ForwardVector = ReverseRotation.RotateVector(FVector::ForwardVector);

	FVector StartLocation = WallHitResult.ImpactPoint - (ForwardVector * 30.0f);
	FVector EndLocation = WallHitResult.ImpactPoint + (ForwardVector * 30.0f);

	// Trace sphere to get edge location on the wall
	PerformSphereTraceByChannel(Character->GetWorld(), ClimbLedgeResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug);

	// If nothing blocks trace then edge is not valid
	if (!ClimbLedgeResult.bBlockingHit)
		return;

	// Set wall rotation depending on trace result
	WallRotation = UParkourFunctionLibrary::NormalReverseRotationZ(ClimbLedgeResult.ImpactNormal);
	ForwardVector = WallRotation.RotateVector(FVector::ForwardVector);

	StartLocation = ClimbLedgeResult.ImpactPoint + (ForwardVector * 2.0f) + FVector(0.0f, 0.0f, 5.0f);
	EndLocation = StartLocation - FVector(0.0f, 0.0f, 55.0f);

	// Trace sphere to get location of the top of the wall and its height
	// Here debug nicely shows where is the origin ledge and target ledge
	FHitResult HitResult;
	PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug);

	ClimbLedgeResult.Location = HitResult.Location;
	ClimbLedgeResult.ImpactPoint.Z = HitResult.ImpactPoint.Z;
}

void UParkourComponent::LeftClimbIK()
{
	FHitResult LedgeResult = SecondClimbLedgeResult;
	if (!ParkourStateTag.GetTagName().IsEqual("Parkour.State.ReachLedge"))
		return;

	if (!LedgeResult.bBlockingHit)
		return;

	HandLedgeIK(LedgeResult, true);
	SetFootIK(LedgeResult, true);
}

void UParkourComponent::RightClimbIK()
{
	FHitResult LedgeResult = SecondClimbLedgeResult;
	if (!ParkourStateTag.GetTagName().IsEqual("Parkour.State.ReachLedge"))
		return;

	if (!LedgeResult.bBlockingHit)
		return;

	HandLedgeIK(LedgeResult, false);
	SetFootIK(LedgeResult, false);
}

void UParkourComponent::HandLedgeIK(FHitResult& LedgeResult, bool bIsLeft)
{
	float RightOffsetMultiplier = bIsLeft ? 1.0f : -1.0f;

	// Loop to get front side of the wall edge (this loop represents columns)
	bool bShouldBreak = false;
	for (int32 i = 4; i > -1; i--)
	{
		FVector ForwardVector = WallRotation.RotateVector(FVector::ForwardVector);
		FVector RightVector = WallRotation.RotateVector(FVector::RightVector);


		FVector StartLocation = LedgeResult.ImpactPoint - ForwardVector * 20.0f - RightVector * (2.0f + i * 2.0f) * RightOffsetMultiplier;
		FVector EndLocation = StartLocation + ForwardVector * 40;

		// Trace sphere to get front side of the wall edge
		// Trace get further to left after each iteration
		FHitResult FirstHitResult;
		PerformSphereTraceByChannel(Character->GetWorld(), FirstHitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug);

		if (!FirstHitResult.bBlockingHit)
			continue;

		// Loop to get and set hand location and rotation (this loop represents rows)
		for (int32 j = 0; j < 6; j++)
		{
			FRotator HitResultReverseRotator = UParkourFunctionLibrary::NormalReverseRotationZ(FirstHitResult.ImpactNormal);
			ForwardVector = HitResultReverseRotator.RotateVector(FVector::ForwardVector);

			StartLocation = FirstHitResult.ImpactPoint + ForwardVector * 2.0f;
			StartLocation.Z += j * 5;

			EndLocation = StartLocation;
			EndLocation.Z -= 50 + j * 5;

			// Trace sphere to get hand lcoation on the edge on top
			// Trace get higher each iteration
			FHitResult SecondHitResult;
			PerformSphereTraceByChannel(Character->GetWorld(), SecondHitResult, StartLocation, EndLocation, 2.5f, ECC_Visibility, bDrawDebug);


			if (!SecondHitResult.bStartPenetrating)
			{
				if (SecondHitResult.bBlockingHit)
				{
					SetHandIK(FirstHitResult, SecondHitResult, bIsLeft, false);

					bShouldBreak = true;
					break;
				}
			}
			else if (i == 0)
			{
				SetHandIK(FirstHitResult, SecondHitResult, bIsLeft, true);

			}
		}

		if (bShouldBreak)
			break;
	}

	//TODO fix when hand could not find location
	//solution copy another hand location
}

void UParkourComponent::SetHandIK(const FHitResult& FirstHitResult, const FHitResult& SecondHitResult, bool bIsLeft, bool bIsFinal)
{
	FRotator HitResultReverseRotator = UParkourFunctionLibrary::NormalReverseRotationZ(FirstHitResult.ImpactNormal);
	FVector ForwardVector = HitResultReverseRotator.RotateVector(FVector::ForwardVector);

	// If it is final iteration, then valid second hit result wasn't found, should be used simplified version
	FVector HandLedgeLocation;
	if (!bIsFinal)
	{
		float StyleMultiplier = ClimbStyle.GetTagName().IsEqual("Parkour.ClimbStyle.Braced") ? CharacterHandFront : 0.0f;
		HandLedgeLocation = FirstHitResult.ImpactPoint + ForwardVector * (-3.0f + StyleMultiplier);
		HandLedgeLocation.Z = SecondHitResult.ImpactPoint.Z + CharacterHeightDiff + CharacterHandUp - 9.0f;
	}
	else
	{
		HandLedgeLocation = FirstHitResult.ImpactPoint;
		HandLedgeLocation.Z -= 9.0f;
	}

	// Set different rotation depending on which hand is setting
	FRotator HandLedgeRotation = HitResultReverseRotator;
	HandLedgeRotation.Roll -= bIsLeft ? 80.0f : 90.0f;
	HandLedgeRotation.Pitch -= bIsLeft ? -90.0f : 90.0f;

	if (!AnimInstance->GetClass()->ImplementsInterface(UParkourABPInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("HandLedgeIK: AnimInstance does not implement the ABP interface"));
		return;
	}

	IParkourABPInterface* ParkourABPInterface = Cast<IParkourABPInterface>(AnimInstance);

	if (bIsLeft)
	{
		LeftHandLedgeLocation = HandLedgeLocation;
		LeftHandLedgeRotation = HandLedgeRotation;

		ParkourABPInterface->Execute_SetLeftHandLedgeLocation(AnimInstance, HandLedgeLocation);
		ParkourABPInterface->Execute_SetLeftHandLedgeRotation(AnimInstance, HandLedgeRotation);
	}
	else
	{
		RightHandLedgeLocation = HandLedgeLocation;
		RightHandLedgeRotation = HandLedgeRotation;

		ParkourABPInterface->Execute_SetRightHandLedgeLocation(AnimInstance, HandLedgeLocation);
		ParkourABPInterface->Execute_SetRightHandLedgeRotation(AnimInstance, HandLedgeRotation);
	}
}

void UParkourComponent::SetFootIK(FHitResult& LedgeResult, bool bIsLeft, bool bIsSimplified /*= false*/)
{
	if (!ClimbStyle.GetTagName().IsEqual("Parkour.ClimbStyle.Braced"))
		return;

	float RightOffsetMultiplier = bIsLeft ? 1.0f : -1.0f;

	int32 Index = 0;

	if (bIsSimplified)
	{
		Index = 5;
		RightOffsetMultiplier = 0.0f;
	}
	else
		Index = 3;

	// Loop to get front side of the wall edge (this loop represents columns)
	for (int32 i = 0; i < Index; i++)
	{
		FVector ForwardVector = WallRotation.RotateVector(FVector::ForwardVector);
		FVector RightVector = WallRotation.RotateVector(FVector::RightVector);

		FVector StartLocation = LedgeResult.ImpactPoint - (ForwardVector * 30.0f) - (RightVector * 7.0f) * RightOffsetMultiplier;
		StartLocation.Z += i * 5.0f + (CharacterHeightDiff - 150.0f);
		FVector EndLocation = StartLocation + (ForwardVector * 60.0f);

		// Trace to get foot location on the wall
		// Trace gets higher after each iteration
		FHitResult HitResult;
		PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 6.0f, ECC_Visibility, bDrawDebug);

		// If something blocking trace then place is valid for foot
		if (!HitResult.bBlockingHit)
			continue;

		FRotator NewRotation = UParkourFunctionLibrary::NormalReverseRotationZ(HitResult.ImpactNormal);
		FVector NewForwardVector = NewRotation.RotateVector(FVector::ForwardVector);

		FVector FootLocation = HitResult.ImpactPoint - (ForwardVector * 17.0f);

		if (!AnimInstance->GetClass()->ImplementsInterface(UParkourABPInterface::StaticClass()))
		{
			UE_LOG(LogTemp, Warning, TEXT("LeftFootIK: AnimInstance does not implement the ABP interface"));
			return;
		}

		IParkourABPInterface* ParkourABPInterface = Cast<IParkourABPInterface>(AnimInstance);

		if (bIsLeft)
			ParkourABPInterface->Execute_SetLeftFootLocation(AnimInstance, FootLocation);
		else
			ParkourABPInterface->Execute_SetRightFootLocation(AnimInstance, FootLocation);

		return;
	}

	if (bIsSimplified)
		return;

	SetFootIK(LedgeResult, bIsLeft, true);
}

void UParkourComponent::ClimbMoveIK()
{
	if (!ParkourStateTag.GetTagName().IsEqual("Parkour.State.Climb"))
		return;

	ClimbMoveHandIK();
	ClimbMoveFootIK();
}

void UParkourComponent::ClimbMoveHandIK()
{
	if (!ParkourStateTag.GetTagName().IsEqual("Parkour.State.Climb"))
		return;

	UpdateClimbMoveHandIK(true);
	UpdateClimbMoveHandIK(false);

}

void UParkourComponent::UpdateClimbMoveHandIK(bool bIsLeft)
{
	float RightOffsetMultiplier = bIsLeft ? 1.0f : -1.0f;

	FName HandSocketName = bIsLeft ? "ik_hand_l" : "ik_hand_r";

	FHitResult FirstHitResult;

	// Loop to get front side of the wall edge in front of a hand
	for (int32 i = 0; i < 5; i++)
	{
		FVector InitialVector = CharacterMesh->GetSocketLocation(HandSocketName) + Character->GetActorRightVector() * (i * 2.0f + ClimbHandSpace) * RightOffsetMultiplier;
		InitialVector.Z -= CharacterHeightDiff;

		FVector StartLocation = InitialVector - Character->GetActorForwardVector() * 20.0f;
		FVector EndLocation = StartLocation + Character->GetActorForwardVector() * 35.0f;

		// Trace sphere to get wall front side from hand IK socket
		// Trace gets further to side from hand position after each iteration
		PerformSphereTraceByChannel(Character->GetWorld(), FirstHitResult, StartLocation, EndLocation, 15.0f, ECC_Visibility, bDrawDebug, 0.0f);

		// If nothing blocks trace, then there is no wall in front of the hand
		if (FirstHitResult.bStartPenetrating || !FirstHitResult.bBlockingHit)
		{
			if (i == 4)
				return;

			continue;
		}

		break;
	}

	FHitResult SecondHitResult;

	// Loop to get top side of the wall edge for hand
	for (int32 i = 0; i < 6; i++)
	{
		FVector StartLocation = FirstHitResult.ImpactPoint;
		StartLocation.Z = ArrowActor->GetArrowComponent()->GetComponentLocation().Z + 10.0f + i * 5.0f;

		FVector EndLocation = StartLocation;
		EndLocation.Z -= 5 * (i + 10);

		// Trace sphere to check whether wall top side has enough space for hand
		// Trace gets higher after each iteration
		PerformSphereTraceByChannel(Character->GetWorld(), SecondHitResult, StartLocation, EndLocation, 2.5f, ECC_Visibility, bDrawDebug, 0.0f);

		// If hit result is valid then break the loop (it will be hand location)
		// If hit result is not valid and it is last iteration, then no need to check futrther wall is not valid
		if (!SecondHitResult.bStartPenetrating)
		{
			if (!SecondHitResult.bBlockingHit)
				return;

			break;
		}
		else if (i == 5)
			return;
	}

	FRotator XRotation = FRotationMatrix::MakeFromX(FirstHitResult.ImpactNormal).Rotator();
	FVector ForwardVector = XRotation.RotateVector(FVector::ForwardVector);

	FVector HandLocation = FirstHitResult.ImpactPoint - ForwardVector * (UParkourFunctionLibrary::SelectClimbStyleFloat(CharacterHandFront, 0.0f, ClimbStyle) - 3.0f);
	HandLocation.Z = SecondHitResult.ImpactPoint.Z + CharacterHeightDiff + CharacterHandUp + GetClimbLeftHandZOffset();
	HandLocation.Z -= UParkourFunctionLibrary::SelectClimbStyleFloat(9.0f, 8.0f, ClimbStyle);

	FRotator HandRotation = FRotationMatrix::MakeFromX(FirstHitResult.ImpactNormal).Rotator();
	UParkourFunctionLibrary::ReverseRotation(HandRotation);

	// Set different rotation for different hands
	HandRotation.Roll -= bIsLeft ? 80.0f : 90.0f;
	HandRotation.Pitch -= bIsLeft ? -90.0f : 90.0f;


	if (!AnimInstance->GetClass()->ImplementsInterface(UParkourABPInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("ClimbMoveLeftHandIK: AnimInstance does not implement the ABP interface"));
		return;
	}

	IParkourABPInterface* ParkourABPInterface = Cast<IParkourABPInterface>(AnimInstance);

	if (bIsLeft)
	{
		ParkourABPInterface->Execute_SetLeftHandLedgeLocation(AnimInstance, HandLocation);
		ParkourABPInterface->Execute_SetLeftHandLedgeRotation(AnimInstance, HandRotation);
	}
	else
	{
		ParkourABPInterface->Execute_SetRightHandLedgeLocation(AnimInstance, HandLocation);
		ParkourABPInterface->Execute_SetRightHandLedgeRotation(AnimInstance, HandRotation);
	}
}

void UParkourComponent::ClimbMoveFootIK()
{
	if (!ParkourStateTag.GetTagName().IsEqual("Parkour.State.Climb"))
		return;

	UpdateClimbMoveFootIK(true);
	UpdateClimbMoveFootIK(false);
}

void UParkourComponent::UpdateClimbMoveFootIK(bool bIsLeft)
{
	FName CurveName = bIsLeft ? "LeftFootIK" : "RightFootIK";

	// If foot curve value in animation is not 1 (IK actovated), then no need to calculate foot position
	if (AnimInstance->GetCurveValue(CurveName) != 1.0f)
	{
		ResetFootIK(true);
		return;
	}

	FName FootSocketName = bIsLeft ? "ik_foot_l" : "ik_foot_r";
	FName HandSocketName = bIsLeft ? "hand_l" : "hand_r";

	float UpOffset = bIsLeft ? 135.0f : 125.0f;
	float RightOffsetMultiplier = bIsLeft ? 1.0f : -1.0f;

	FHitResult FootHitResult;

	// Outer loop to get foot location on the wall
	bool bShouldBreak = false;
	for (int32 i = 0; i < 5; i++)
	{
		// Inner loop to get wall in front of the foot 
		for (int32 j = 0; j < 5; j++)
		{
			FVector StartLocation = CharacterMesh->GetSocketLocation(FootSocketName);
			StartLocation.Z = CharacterMesh->GetSocketLocation(HandSocketName).Z + (j * 5.0f) - UpOffset;
			StartLocation += Character->GetActorRightVector() * (13.0f + i * 4.0f) * RightOffsetMultiplier;
			StartLocation -= Character->GetActorForwardVector() * 30.0f;

			FVector EndLocation = StartLocation + Character->GetActorForwardVector() * 100.0f;

			// Trace sphere to get wall in front of the foot 
			// Trace gets further to side from foot location after each iteration of outer loop
			// Trace gets lower after each iteration of inner loop
			PerformSphereTraceByChannel(Character->GetWorld(), FootHitResult, StartLocation, EndLocation, 15.0f, ECC_Visibility, bDrawDebug, 0.0f);

			// If nothing blocks trace, then there is no valid wall 
			if (!FootHitResult.bBlockingHit || FootHitResult.bStartPenetrating)
				continue;

			bShouldBreak = true;
			break;
		}

		if (bShouldBreak)
			break;
	}

	// If outer loop ended naturally (bShouldBreak wasn't triggered), then there is no valid result
	if (!bShouldBreak)
		return;

	FRotator XRotation = FRotationMatrix::MakeFromX(FootHitResult.ImpactNormal).Rotator();
	FVector ForwardVector = XRotation.RotateVector(FVector::ForwardVector);

	FVector FootLocation = FootHitResult.ImpactPoint + ForwardVector * 18.0f;

	if (!AnimInstance->GetClass()->ImplementsInterface(UParkourABPInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("ClimbMoveLeftFootIK: AnimInstance does not implement the ABP interface"));
		return;
	}

	IParkourABPInterface* ParkourABPInterface = Cast<IParkourABPInterface>(AnimInstance);

	if (bIsLeft)
		ParkourABPInterface->Execute_SetLeftFootLocation(AnimInstance, FootLocation);
	else
		ParkourABPInterface->Execute_SetRightFootLocation(AnimInstance, FootLocation);
}

void UParkourComponent::ResetFootIK(bool bIsLeft)
{
	if (!AnimInstance->GetClass()->ImplementsInterface(UParkourABPInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("ResetFootIK: AnimInstance does not implement the ABP interface"));
		return;
	}

	IParkourABPInterface* ParkourABPInterface = Cast<IParkourABPInterface>(AnimInstance);

	// Set standart foot ik location
	if (bIsLeft)
		ParkourABPInterface->Execute_SetLeftFootLocation(AnimInstance, CharacterMesh->GetSocketLocation("ik_foot_l"));
	else
		ParkourABPInterface->Execute_SetRightFootLocation(AnimInstance, CharacterMesh->GetSocketLocation("ik_foot_r"));
}

void UParkourComponent::PlayParkourMontage()
{
	// Check whether parkour data asset was loaded corectly
	checkf(ParkourVariables != nullptr, TEXT("ParkourVariables are not initialized for \"%s\" action"), *ParkourActionTag.GetTagName().ToString());

	SetParkourState(ParkourVariables->ParkourInState);

	// FirstTopResultOffset used for any edge interaction (like reach ledge, climb on the ledge, hop on the ledge, etc.)
	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation("FirstTopResultOffset",
		FindWarpLocation(WallTopResult.ImpactPoint, ParkourVariables->Warp1XOffset, ParkourVariables->Warp1ZOffset), WallRotation);

	// DepthResultOffset used for vaults
	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation("DepthResultOffset",
		FindWarpLocation(WallDepthResult.ImpactPoint, ParkourVariables->Warp2XOffset, ParkourVariables->Warp2ZOffset), WallRotation);

	// VaultResultOffset used for vaults
	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation("VaultResultOffset",
		FindWarpLocation(WallVaultResult.ImpactPoint, ParkourVariables->Warp3XOffset, ParkourVariables->Warp3ZOffset), WallRotation);

	// CheckedTopResultOffset used for free hang climbing up and mantles
	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation("CheckedTopResultOffset",
		FindWarpLocationChecked(WallTopResult.ImpactPoint, ParkourVariables->Warp2XOffset, ParkourVariables->Warp2ZOffset), WallRotation);

	// SecondTopResultOffset used for falling climb and for free hang initial climb (second offset is usefull for changing position two times per montage)
	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation("SecondTopResultOffset",
		FindWarpLocation(WallTopResult.ImpactPoint, ParkourVariables->Warp2XOffset, ParkourVariables->Warp2ZOffset), WallRotation);

	UAnimMontage* AnimMontage = ParkourVariables->ParkourMontage;
	if (!AnimMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayParkourMontage: AnimMontage wasn't found"));
		return;
	}
	float StartTimeInSeconds = MontageStartTime;

	AnimInstance->Montage_Play(AnimMontage, 1.0f, EMontagePlayReturnType::MontageLength, StartTimeInSeconds);
}

FVector UParkourComponent::FindWarpLocation(const FVector& ImpactPoint, float XOffset, float ZOffset) const
{
	FVector Result = ImpactPoint;
	Result.Z += ZOffset;

	Result += WallRotation.RotateVector(FVector::ForwardVector) * XOffset;

	return Result;
}

FVector UParkourComponent::FindWarpLocationChecked(const FVector& ImpactPoint, float XOffset, float ZOffset) const
{
	FVector StartLocation = ImpactPoint + WallRotation.RotateVector(FVector::ForwardVector) * XOffset + FVector(0.0f, 0.0f, 40.0f);
	FVector EndLocation = StartLocation - FVector(0.0f, 0.0f, 60.0f);

	// Trace sphere to check whether place with offset is valid
	FHitResult HitResult;
	PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 25.0f, ECC_Visibility, bDrawDebug);

	FVector AdjustedImpactPoint = HitResult.bBlockingHit ? HitResult.ImpactPoint : ImpactPoint;

	AdjustedImpactPoint.Z += ZOffset;

	return AdjustedImpactPoint;
}

void UParkourComponent::FindMontageStartTime()
{
	if ((ParkourActionTag.GetTagName().IsEqual("Parkour.Action.Climb") || ParkourActionTag.GetTagName().IsEqual("Parkour.Action.FreeHangClimb")) && !bOnGround)
	{
		MontageStartTime = ParkourVariables->FallingMontageStartPosition;
		return;
	}

	MontageStartTime = ParkourVariables->MontageStartPosition;
}

void UParkourComponent::PreinitializeParkourDataAssets(FParkourVariablesCollection& ParkourCollection) const
{
	// Preset all parkour data assets

	static ConstructorHelpers::FObjectFinder<UParkourVariables> BracedClimbDTObj(TEXT("/ParkourSystemPlugin/DataAssets/DT_BracedClimb"));
	ParkourCollection.BracedClimbDT = BracedClimbDTObj.Object;

	static ConstructorHelpers::FObjectFinder<UParkourVariables> BracedClimbUpDTObj(TEXT("/ParkourSystemPlugin/DataAssets/DT_BracedClimbUp"));
	ParkourCollection.BracedClimbUpDT = BracedClimbUpDTObj.Object;

	static ConstructorHelpers::FObjectFinder<UParkourVariables> BracedDropDownDTObj(TEXT("/ParkourSystemPlugin/DataAssets/DT_BracedDropDown"));
	ParkourCollection.BracedDropDownDT = BracedDropDownDTObj.Object;

	static ConstructorHelpers::FObjectFinder<UParkourVariables> ClimbHopDownDTObj(TEXT("/ParkourSystemPlugin/DataAssets/DT_ClimbHopDown"));
	ParkourCollection.ClimbHopDownDT = ClimbHopDownDTObj.Object;

	static ConstructorHelpers::FObjectFinder<UParkourVariables> ClimbHopLeftDTObj(TEXT("/ParkourSystemPlugin/DataAssets/DT_ClimbHopLeft"));
	ParkourCollection.ClimbHopLeftDT = ClimbHopLeftDTObj.Object;

	static ConstructorHelpers::FObjectFinder<UParkourVariables> ClimbHopLeftUpDTObj(TEXT("/ParkourSystemPlugin/DataAssets/DT_ClimbHopLeftUp"));
	ParkourCollection.ClimbHopLeftUpDT = ClimbHopLeftUpDTObj.Object;

	static ConstructorHelpers::FObjectFinder<UParkourVariables> ClimbHopRightDTObj(TEXT("/ParkourSystemPlugin/DataAssets/DT_ClimbHopRight"));
	ParkourCollection.ClimbHopRightDT = ClimbHopRightDTObj.Object;

	static ConstructorHelpers::FObjectFinder<UParkourVariables> ClimbHopRightUpDTObj(TEXT("/ParkourSystemPlugin/DataAssets/DT_ClimbHopRightUp"));
	ParkourCollection.ClimbHopRightUpDT = ClimbHopRightUpDTObj.Object;

	static ConstructorHelpers::FObjectFinder<UParkourVariables> ClimbHopUpDTObj(TEXT("/ParkourSystemPlugin/DataAssets/DT_ClimbHopUp"));
	ParkourCollection.ClimbHopUpDT = ClimbHopUpDTObj.Object;

	static ConstructorHelpers::FObjectFinder<UParkourVariables> FallingBracedClimbDTObj(TEXT("/ParkourSystemPlugin/DataAssets/DT_FallingBracedClimb"));
	ParkourCollection.FallingBracedClimbDT = FallingBracedClimbDTObj.Object;

	static ConstructorHelpers::FObjectFinder<UParkourVariables> FallingFreeHangClimbDTObj(TEXT("/ParkourSystemPlugin/DataAssets/DT_FallingFreeHangClimb"));
	ParkourCollection.FallingFreeHangClimbDT = FallingFreeHangClimbDTObj.Object;

	static ConstructorHelpers::FObjectFinder<UParkourVariables> FreeClimbHopDownDTObj(TEXT("/ParkourSystemPlugin/DataAssets/DT_FreeClimbHopDown"));
	ParkourCollection.FreeClimbHopDownDT = FreeClimbHopDownDTObj.Object;

	static ConstructorHelpers::FObjectFinder<UParkourVariables> FreeClimbHopLeftDTObj(TEXT("/ParkourSystemPlugin/DataAssets/DT_FreeClimbHopLeft"));
	ParkourCollection.FreeClimbHopLeftDT = FreeClimbHopLeftDTObj.Object;

	static ConstructorHelpers::FObjectFinder<UParkourVariables> FreeClimbHopRightDTObj(TEXT("/ParkourSystemPlugin/DataAssets/DT_FreeClimbHopRight"));
	ParkourCollection.FreeClimbHopRightDT = FreeClimbHopRightDTObj.Object;

	static ConstructorHelpers::FObjectFinder<UParkourVariables> FreeHangClimbUpDTObj(TEXT("/ParkourSystemPlugin/DataAssets/DT_FreeHangClimbUp"));
	ParkourCollection.FreeHangClimbUpDT = FreeHangClimbUpDTObj.Object;

	static ConstructorHelpers::FObjectFinder<UParkourVariables> FreeHangDropDownDTObj(TEXT("/ParkourSystemPlugin/DataAssets/DT_FreeHangDropDown"));
	ParkourCollection.FreeHangDropDownDT = FreeHangDropDownDTObj.Object;

	static ConstructorHelpers::FObjectFinder<UParkourVariables> FreeHangDTObj(TEXT("/ParkourSystemPlugin/DataAssets/DT_FreeHang"));
	ParkourCollection.FreeHangDT = FreeHangDTObj.Object;

	static ConstructorHelpers::FObjectFinder<UParkourVariables> HighVaultDTObj(TEXT("/ParkourSystemPlugin/DataAssets/DT_HighVault"));
	ParkourCollection.HighVaultDT = HighVaultDTObj.Object;

	static ConstructorHelpers::FObjectFinder<UParkourVariables> LowMantleDTObj(TEXT("/ParkourSystemPlugin/DataAssets/DT_LowMantle"));
	ParkourCollection.LowMantleDT = LowMantleDTObj.Object;

	static ConstructorHelpers::FObjectFinder<UParkourVariables> MantleDTObj(TEXT("/ParkourSystemPlugin/DataAssets/DT_Mantle"));
	ParkourCollection.MantleDT = MantleDTObj.Object;

	static ConstructorHelpers::FObjectFinder<UParkourVariables> ThinVaultDTObj(TEXT("/ParkourSystemPlugin/DataAssets/DT_ThinVault"));
	ParkourCollection.ThinVaultDT = ThinVaultDTObj.Object;

	static ConstructorHelpers::FObjectFinder<UParkourVariables> VaultDTObj(TEXT("/ParkourSystemPlugin/DataAssets/DT_Vault"));
	ParkourCollection.VaultDT = VaultDTObj.Object;
}

void UParkourComponent::OnParkourMontageBlendOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
		return;

	SetParkourState(ParkourVariables->ParkourOutState);
	SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.NoAction")));
}