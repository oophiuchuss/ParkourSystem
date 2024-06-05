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

#include "ParkourFunctionLibrary.h"
#include "ParkourABPInterface.h"
#include "ParkourStatsInterface.h"

#include "ThinVaultDT.h"
#include "VaultDT.h"
#include "HighVaultDT.h"
#include "MantleDT.h"
#include "LowMantleDT.h"
#include "BracedClimbDT.h"
#include "FreeHangDT.h"
#include "BracedClimbUpDT.h"
#include "FreeHangClimbUpDT.h"
#include "FallingBracedClimbDT.h"
#include "FallingFreeHangClimb.h"
#include "BracedDropDownDT.h"
#include "FreeHangDropDownDT.h"
#include "ClimbHopUpDT.h"
#include "ClimbHopLeftDT.h"
#include "ClimbHopRightDT.h"
#include "ClimbHopLeftUpDT.h"
#include "ClimbHopRightUpDT.h"
#include "ClimbHopDownDT.h"
#include "FreeClimbHopLeftDT.h"
#include "FreeClimbHopRightDT.h"
#include "FreeClimbHopDownDT.h"

// Sets default values for this component's properties
UParkourComponent::UParkourComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	FString path = "/ParkourSystemPlugin/Curves/FC_ParkourCameraMove";
	ConstructorHelpers::FObjectFinder<UCurveFloat> MontageAsset(*path);

	if (MontageAsset.Succeeded())
		CameraCurve = MontageAsset.Object;
	else
		UE_LOG(LogTemp, Warning, TEXT("UParkourComponent: CameraCurve wasn't found"));
}

// Called when the game starts
void UParkourComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

// Called every frame
void UParkourComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AutoClimb();

	ClimbMoveIK();
}

bool UParkourComponent::SetInitializeReference(ACharacter* NewCharacter, USpringArmComponent* NewCameraBoom, UMotionWarpingComponent* NewMotionWarping, UCameraComponent* NewCamera)
{
	Character = NewCharacter;
	CharacterMovement = Character->GetCharacterMovement();
	CharacterMesh = Character->GetMesh();
	AnimInstance = CharacterMesh->GetAnimInstance();
	CapsuleComponent = Character->GetCapsuleComponent();
	CameraBoom = NewCameraBoom;
	MotionWarping = NewMotionWarping;
	Camera = NewCamera;
	ParkourActionTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.NoAction"));
	ParkourStateTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.State.NotBusy"));
	ClimbStyle = UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.ClimbStyle.FreeHang"));
	FGameplayTag NoDirectionTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.NoDirection"));
	ClimbDirection = NoDirectionTag;
	ClimbMoveCheckDistance = 10.0f;
	ClimbHandSpace = 20.0f;
	bCanAutoClimb = true;
	bCanManualClimb = true;
	bShowHitResult = true;
	bDrawDebug = false;
	bOnGround = true;
	if (Character)
	{
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
			WidgetActor->SetActorRelativeLocation(FVector(100.0f, 50.0f, -3.0f));
		}
		else
			return false;

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
			ArrowActor->SetActorRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
		}
		else
			return false;



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

	if (AnimInstance)
		AnimInstance->OnMontageBlendingOut.AddDynamic(this, &UParkourComponent::OnParkourMontageBlendOut);



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
	ParkourActionFunction(false);
}

void UParkourComponent::ParkourActionFunction(bool bAutoClimb)
{
	if (!ParkourActionTag.GetTagName().IsEqual("Parkour.Action.NoAction"))
		return;

	bool bCheckClimb = bAutoClimb ? bCanAutoClimb : bCanManualClimb;

	if (!bCheckClimb)
		return;

	ChekcWallShape();
	ShowHitResults();
	CheckDistance();
	ParkourType(bAutoClimb);
}

void UParkourComponent::AutoClimb()
{
	FVector Location = CharacterMesh->GetSocketLocation("root");

	if (ParkourStateTag.GetTagName().IsEqual("Parkour.State.Climb"))
		Location.Z += UParkourFunctionLibrary::SelectClimbStyleFloat(50.0f, 2.0f, ClimbStyle);

	FHitResult HitResult;
	PerformBoxTraceByChannel(Character->GetWorld(), HitResult, Location, Location, FVector(10.0f, 10.0f, 4.0f), ECC_Visibility, bDrawDebug, 0.0f);
	bOnGround = (HitResult.bBlockingHit || HitResult.bStartPenetrating);

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
	if (!bOnGround)
	{
		if (ParkourStateTag.GetTagName().IsEqual("Parkour.State.Climb"))
		{
			SetParkourState(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.State.NotBusy")));
			bCanManualClimb = false;
			bCanAutoClimb = false;

			FTimerManager& TimerManager = GetWorld()->GetTimerManager();

			float Delay = 0.3f;
			TimerManager.SetTimer(TimerHandle_DelayedFunction, this, &UParkourComponent::SetCanManualClimb, Delay, false);
		}
	}
	else
	{
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

void UParkourComponent::ChekcWallShape()
{
	FHitResult HitResult;
	FHitResult LoopTraceHitResult;
	FHitResult InnerLoopTraceHitResult;

	FVector StartLocation;
	FVector EndLocation;

	int32 Index = CharacterMovement->IsFalling() ? 8 : 15;

	bool bShouldBreak = false;
	for (int32 i = 0; i <= Index; i++)
	{
		for (int32 j = 0; j <= 11; j++)
		{
			StartLocation = Character->GetActorLocation() + FVector(0.0f, 0.0f, (i * 16.0f) + FirstTraceHeight()) +
				(Character->GetActorForwardVector() * (-20.0f));
			EndLocation = Character->GetActorLocation() + FVector(0.0f, 0.0f, (i * 16.0f) + FirstTraceHeight()) +
				(Character->GetActorForwardVector() * (j * 10.0f + 10.0f));

			PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 10.0f, ECC_Visibility, bDrawDebug);

			if (HitResult.bBlockingHit && !HitResult.bStartPenetrating)
			{
				bShouldBreak = true;
				break;
			}
		}

		if (bShouldBreak)
			break;
	}


	if (!HitResult.bBlockingHit || HitResult.bStartPenetrating)
		return;


	WallHitTraces.Empty();

	Index = UParkourFunctionLibrary::SelectParkourStateFloat(4.0f, .0f, .0f, 2.0f, ParkourStateTag);

	for (int32 i = 0; i <= Index; i++)
	{
		FVector ImpactPoint = HitResult.ImpactPoint;
		if (!ParkourStateTag.GetTagName().IsEqual("Parkour.State.Climb"))
			ImpactPoint.Z = Character->GetActorLocation().Z - 60.0f;

		FQuat QuatRotation = FQuat(UParkourFunctionLibrary::NormalReverseRotationZ(HitResult.ImpactNormal));
		FVector RightVector = QuatRotation.RotateVector(FVector::RightVector);
		FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

		float Offset = (i * 20.0f + UParkourFunctionLibrary::SelectParkourStateFloat(-40.0f, 0.0f, 0.0f, -20.0f, ParkourStateTag));

		StartLocation = (RightVector * Offset) + ImpactPoint + (ForwardVector * -40.0f);
		EndLocation = (RightVector * Offset) + ImpactPoint + (ForwardVector * 30.0f);

		PerformLineTraceByChannel(Character->GetWorld(), LoopTraceHitResult, StartLocation, EndLocation, ECC_Visibility, bDrawDebug);

		HopHitTraces.Empty();

		HopHitTraces.Add(LoopTraceHitResult);

		int InnerIndex = UParkourFunctionLibrary::SelectParkourStateFloat(29.0f, .0f, .0f, 6.0f, ParkourStateTag);

		for (int32 k = 0; k <= InnerIndex; k++)
		{
			StartLocation.Z += 8.0f;
			EndLocation.Z += 8.0f;

			PerformLineTraceByChannel(Character->GetWorld(), InnerLoopTraceHitResult, StartLocation, EndLocation, ECC_Visibility, bDrawDebug);

			HopHitTraces.Add(InnerLoopTraceHitResult);
		}

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

	if (WallHitTraces.Num() != 0)
	{
		if (WallHitTraces.Num() > 0)
		{
			WallHitResult = WallHitTraces[0];

			for (int32 i = 1; i < WallHitTraces.Num(); i++)
			{
				float Distance1 = FVector::Distance(Character->GetActorLocation(), WallHitTraces[i].ImpactPoint);
				float Distance2 = FVector::Distance(Character->GetActorLocation(), WallHitResult.ImpactPoint);
				if (Distance1 <= Distance2)
				{
					WallHitResult = WallHitTraces[i];
				}
			}
		}

		if (WallHitResult.bBlockingHit && !WallHitResult.bStartPenetrating)
		{

			if (!ParkourStateTag.GetTagName().IsEqual("Parkour.State.Climb"))
			{
				WallRotation = UParkourFunctionLibrary::NormalReverseRotationZ(WallHitResult.ImpactNormal);
			}

			for (int32 i = 0; i <= 8; i++)
			{
				FQuat QuatRotation = FQuat(WallRotation);
				FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

				StartLocation = (ForwardVector * (i * 30.0f + 2.0f)) + WallHitResult.ImpactPoint + FVector(0.0f, 0.0f, 7.0f);
				EndLocation = StartLocation - FVector(0.0f, 0.0f, 7.0f);

				PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 2.5f, ECC_Visibility, bDrawDebug);

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

			if (ParkourStateTag.GetTagName().IsEqual("Parkour.State.NotBusy"))
			{
				FQuat QuatRotation = FQuat(WallRotation);
				FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

				StartLocation = TopHits.ImpactPoint + (ForwardVector * 30.0f);
				EndLocation = TopHits.ImpactPoint;

				PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 2.5f, ECC_Visibility, bDrawDebug);

				if (HitResult.bBlockingHit)
				{
					WallDepthResult = HitResult;

					StartLocation = WallDepthResult.ImpactPoint + ForwardVector * 70.0f;
					EndLocation = StartLocation - FVector(0.0f, 0.0f, 200.0f);

					PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 10.0f, ECC_Visibility, bDrawDebug);


					if (HitResult.bBlockingHit)
					{
						WallVaultResult = HitResult;
					}
				}
			}
		}
	}
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

	if (bDrawDebugBox) {
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
		if (HitResult.bBlockingHit)
		{
			DrawDebugLine(World, StartLocation, EndLocation, FColor::Red, false, DrawTime);
			DrawDebugPoint(World, HitResult.ImpactPoint, 10.0f, FColor::Red, false, DrawTime);
		}
		else
			DrawDebugLine(World, StartLocation, EndLocation, FColor::Green, false, DrawTime);
	}
}

void UParkourComponent::ShowHitResults()
{
	if (bShowHitResult)
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
}

void UParkourComponent::CheckDistance()
{
	if (WallHitResult.bBlockingHit)
	{
		WallHeight = .0f;
		WallDepth = .0f;
		VaultHeight = .0f;

		if (WallTopResult.bBlockingHit)
			WallHeight = WallTopResult.ImpactPoint.Z - CharacterMesh->GetSocketLocation("root").Z;
		if (WallTopResult.bBlockingHit && WallDepthResult.bBlockingHit)
			WallDepth = FVector::Distance(WallTopResult.ImpactPoint, WallDepthResult.ImpactPoint);
		if (WallDepthResult.bBlockingHit && WallVaultResult.bBlockingHit)
			VaultHeight = WallDepthResult.ImpactPoint.Z - WallVaultResult.ImpactPoint.Z;

		UE_LOG(LogTemp, Log, TEXT("WallHeight: %f, WallDepth: %f, VaultHeight: %f"), WallHeight, WallDepth, VaultHeight);
	}
}

void UParkourComponent::ParkourType(bool bAutoClimb)
{
	if (!WallTopResult.bBlockingHit)
	{
		SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.NoAction")));
		if (!bAutoClimb)
		{
			Character->Jump();
		}
		return;
	}

	if (!ParkourStateTag.GetTagName().IsEqual("Parkour.State.NotBusy"))
	{
		if (ParkourStateTag.GetTagName().IsEqual("Parkour.State.Climb"))
			CheckClimbOrHop();

		return;
	}

	if (!bOnGround)
	{
		if (CheckClimbSurface())
		{
			CheckClimbStyle();
			SecondClimbLedgeResultCalculation();
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
		return;
	}

	if (WallHeight > 0 && WallHeight <= 44)
	{
		SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.NoAction")));
		return;
	}

	if (WallHeight > 44 && WallHeight < 90)
	{
		if (CheckMantleSurface())
			SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.LowMantle")));
		else
			SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.NoAction")));

		return;
	}

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

	if (WallHeight > 90 && WallHeight <= 160)
		if (CheckMantleSurface())
			SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.Mantle")));
		else
			SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.NoAction")));

	if (WallHeight < 250)
	{
		if (CheckClimbSurface())
		{
			CheckClimbStyle();
			SecondClimbLedgeResultCalculation();
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
	}
}

void UParkourComponent::SetParkourAction(const FGameplayTag& NewParkourAction)
{
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
		ParkourVariables = NewObject<UThinVaultDT>();
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.HighVault"))
	{
		ParkourVariables = NewObject<UHighVaultDT>();
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.Vault"))
	{
		ParkourVariables = NewObject<UVaultDT>();
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.Mantle"))
	{
		ParkourVariables = NewObject<UMantleDT>();
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.LowMantle"))
	{
		ParkourVariables = NewObject<ULowMantleDT>();
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.Climb"))
	{
		ParkourVariables = NewObject<UBracedClimbDT>();
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.FreeHangClimb"))
	{
		ParkourVariables = NewObject<UFreeHangDT>();
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.ClimbingUp"))
	{
		ParkourVariables = NewObject<UBracedClimbUpDT>();
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.FreeHangClimbUp"))
	{
		ParkourVariables = NewObject<UFreeHangClimbUpDT>();
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.FallingBraced"))
	{
		ParkourVariables = NewObject<UFallingBracedClimbDT>();
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.FallingFreeHang"))
	{
		ParkourVariables = NewObject<UFallingFreeHangClimb>();
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.DropDown"))
	{
		ParkourVariables = NewObject<UBracedDropDownDT>();
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.FreeHangDropDown"))
	{
		ParkourVariables = NewObject<UFreeHangDropDownDT>();
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.ClimbHopUp"))
	{
		ParkourVariables = NewObject<UClimbHopUpDT>();
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.ClimbHopLeft"))
	{
		ParkourVariables = NewObject<UClimbHopLeftDT>();
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.ClimbHopRight"))
	{
		ParkourVariables = NewObject<UClimbHopRightDT>();
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.ClimbHopLeftUp"))
	{
		ParkourVariables = NewObject<UClimbHopLeftUpDT>();
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.ClimbHopRightUp"))
	{
		ParkourVariables = NewObject<UClimbHopRightUpDT>();
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.ClimbHopDown"))
	{
		ParkourVariables = NewObject<UClimbHopDownDT>();
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.FreeClimbHopLeft"))
	{
		ParkourVariables = NewObject<UFreeClimbHopLeftDT>();
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.FreeClimbHopRight"))
	{
		ParkourVariables = NewObject<UFreeClimbHopRightDT>();
	}
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.FreeClimbHopDown"))
	{
		ParkourVariables = NewObject<UFreeClimbHopDownDT>();
	}
	else
	{
		ResetParkourResults();
		return;
	}

	BlendOutState = ParkourVariables->ParkourOutState;

	PlayParkourMontage();
}

void UParkourComponent::SetParkourState(const FGameplayTag& NewParkourState)
{
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

void UParkourComponent::SetUpParkourSettings(ECollisionEnabled::Type CollsionType, EMovementMode MovementMode, FRotator RotationRate, bool bDoCollisionTest, bool bStopImmediately)
{
	CapsuleComponent->SetCollisionEnabled(CollsionType);
	CharacterMovement->SetMovementMode(MovementMode);
	CharacterMovement->RotationRate = RotationRate;
	CameraBoom->bDoCollisionTest = bDoCollisionTest;

	if (bStopImmediately)
		CharacterMovement->StopMovementImmediately();
}

void UParkourComponent::PreviousStateSettings(const FGameplayTag& PreviousState, const FGameplayTag& NewState)
{
	if (PreviousState.GetTagName().IsEqual("Parkour.State.Climb"))
	{
		if (NewState.GetTagName().IsEqual("Parkour.State.ReachLedge"))
		{
			//LastClimbRightHandLocation = CharacterMesh->GetSocketLocation("hand_r");
			//LastClimbLeftHandLocation = CharacterMesh->GetSocketLocation("hand_l");
		}
		else if (NewState.GetTagName().IsEqual("Parkour.State.Mantle"))
		{
			TargetRelativeCameraLocation = FirstTargetRelativeLocation;
			TargetArmLenght = FirstCameraTargetArmLenght;
			AddCameraTimeline(0.4f);
		}
		else if (NewState.GetTagName().IsEqual("Parkour.State.NotBusy"))
		{
			TargetRelativeCameraLocation = FirstTargetRelativeLocation;
			TargetArmLenght = FirstCameraTargetArmLenght;
			AddCameraTimeline(0.4f);
		}
	}
	else if (PreviousState.GetTagName().IsEqual("Parkour.State.NotBusy"))
	{
		if (NewState.GetTagName().IsEqual("Parkour.State.ReachLedge"))
		{
			TargetRelativeCameraLocation = FVector(-50.0f, 0.0f, 70.0f);
			TargetArmLenght = 500.0f;
			AddCameraTimeline(0.4f);
		}

	}
}

void UParkourComponent::AddCameraTimeline(float Time)
{
	float WorldDeltaSeconds = GetWorld()->GetDeltaSeconds();
	Time += WorldDeltaSeconds;

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	TimerManager.SetTimer(TimerHandle_FinishCameraTimeline, this, &UParkourComponent::FinishTimeline, 0.4, false);

	TimerManager.SetTimer(TimerHandle_TickCameraTimeline, this, &UParkourComponent::CameraTimelineTick, WorldDeltaSeconds, true);

}

void UParkourComponent::CameraTimelineTick()
{
	float InTime = GetWorld()->GetTimerManager().GetTimerElapsed(TimerHandle_FinishCameraTimeline);
	CameraCurveAlpha = CameraCurve->GetFloatValue(InTime);

	FVector LocationDiff = TargetRelativeCameraLocation - CameraBoom->GetRelativeLocation();

	CameraBoom->SetRelativeLocation(CameraBoom->GetRelativeLocation() + (LocationDiff * CameraCurveAlpha));

	float TargetArmLenghtDiff = TargetArmLenght - CameraBoom->TargetArmLength;

	CameraBoom->TargetArmLength += TargetArmLenghtDiff * CameraCurveAlpha;

}
void UParkourComponent::FinishTimeline()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_FinishCameraTimeline);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_TickCameraTimeline);
	CameraCurveAlpha = 0.0f;
}
bool UParkourComponent::CheckMantleSurface()
{
	FVector StartLocation = WallTopResult.ImpactPoint + FVector(0.0f, 0.0f, CapsuleComponent->GetUnscaledCapsuleHalfHeight() + 8.0f);
	FVector EndLocation = StartLocation;
	float HalfHeight = CapsuleComponent->GetUnscaledCapsuleHalfHeight() - 8.0f;
	float Radius = 25.0f;

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

	FHitResult HitResult;
	PerformCapsuleTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, HalfHeight, Radius, ECC_Visibility, bDrawDebug);

	return !HitResult.bBlockingHit;
}

bool UParkourComponent::CheckClimbSurface()
{
	FQuat QuatRotation = FQuat(WallRotation);
	FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

	FVector StartLocation = WallTopResult.ImpactPoint + FVector(0.0f, 0.0f, -90.0f) + (ForwardVector * -55.0f);
	float HalfHeight = 82.0f;
	float Radius = 25.0f;

	FHitResult HitResult;
	PerformCapsuleTraceByChannel(Character->GetWorld(), HitResult, StartLocation, StartLocation, HalfHeight, Radius, ECC_Visibility, bDrawDebug);

	return !HitResult.bBlockingHit;
}

void UParkourComponent::CheckClimbStyle()
{
	FQuat QuatRotation = FQuat(WallRotation);
	FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

	FVector StartLocation = WallTopResult.ImpactPoint - FVector(0.0f, 0.0f, 125.0f) - (ForwardVector * 10.0f);
	FVector EndLocation = StartLocation + (ForwardVector * 40.0f);
	FHitResult HitResult;
	PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 10.0f, ECC_Visibility, bDrawDebug);

	if (HitResult.bBlockingHit)
		SetClimbStyle(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.ClimbStyle.Braced")));
	else
		SetClimbStyle(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.ClimbStyle.FreeHang")));
}

void UParkourComponent::FirstClimbLedgeResultCalculation()
{
	FQuat QuatRotation = FQuat(UParkourFunctionLibrary::NormalReverseRotationZ(WallHitResult.ImpactNormal));
	FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

	FVector StartLocation = WallHitResult.ImpactPoint - (ForwardVector * 30.0f);
	FVector EndLocation = WallHitResult.ImpactPoint + (ForwardVector * 30.0f);

	PerformSphereTraceByChannel(Character->GetWorld(), FirstClimbLedgeResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug);

	if (!FirstClimbLedgeResult.bBlockingHit)
		return;

	WallRotation = UParkourFunctionLibrary::NormalReverseRotationZ(FirstClimbLedgeResult.ImpactNormal);
	QuatRotation = FQuat(WallRotation);
	ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

	StartLocation = FirstClimbLedgeResult.ImpactPoint + (ForwardVector * 2.0f) + FVector(0.0f, 0.0f, 5.0f);
	EndLocation = StartLocation - FVector(0.0f, 0.0f, 55.0f);

	FHitResult HitResult;
	PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug);

	FirstClimbLedgeResult.Location = HitResult.Location;
	FirstClimbLedgeResult.ImpactPoint.Z = HitResult.ImpactPoint.Z;
}

void UParkourComponent::SecondClimbLedgeResultCalculation()
{
	FQuat QuatRotation = FQuat(UParkourFunctionLibrary::NormalReverseRotationZ(WallHitResult.ImpactNormal));
	FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

	FVector StartLocation = WallHitResult.ImpactPoint - (ForwardVector * 30.0f);
	FVector EndLocation = WallHitResult.ImpactPoint + (ForwardVector * 30.0f);

	PerformSphereTraceByChannel(Character->GetWorld(), SecondClimbLedgeResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug);

	if (!SecondClimbLedgeResult.bBlockingHit)
		return;

	WallRotation = UParkourFunctionLibrary::NormalReverseRotationZ(SecondClimbLedgeResult.ImpactNormal);
	QuatRotation = FQuat(WallRotation);
	ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

	StartLocation = SecondClimbLedgeResult.ImpactPoint + (ForwardVector * 2.0f) + FVector(0.0f, 0.0f, 5.0f);
	EndLocation = StartLocation - FVector(0.0f, 0.0f, 55.0f);

	FHitResult HitResult;
	PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug);

	SecondClimbLedgeResult.Location = HitResult.Location;
	SecondClimbLedgeResult.ImpactPoint.Z = HitResult.ImpactPoint.Z;
}

void UParkourComponent::LeftClimbIK()
{
	FHitResult LedgeResult = SecondClimbLedgeResult;
	if (ParkourStateTag.GetTagName().IsEqual("Parkour.State.ReachLedge"))
	{
		if (LedgeResult.bBlockingHit)
		{
			LeftHandLedgeIK(LedgeResult);

			LeftFootIK(LedgeResult);
		}
	}
}

void UParkourComponent::LeftHandLedgeIK(FHitResult& LedgeResult)
{
	bool bShouldBreak = false;
	for (int32 i = 0; i < 5; i++)
	{

		FQuat QuatRotation = FQuat(WallRotation);
		FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);
		FVector RightVector = QuatRotation.RotateVector(FVector::RightVector);


		FVector StartLocation = LedgeResult.ImpactPoint - ForwardVector * 20.0f - RightVector * (8.0f + i * 2.0f);
		FVector EndLocation = StartLocation + ForwardVector * 40;

		FHitResult FirstHitResult;
		PerformSphereTraceByChannel(Character->GetWorld(), FirstHitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug);

		if (FirstHitResult.bBlockingHit)
		{
			for (int32 j = 0; j < 6; j++)
			{
				FRotator HitResultReverseRotator = UParkourFunctionLibrary::NormalReverseRotationZ(FirstHitResult.ImpactNormal);
				QuatRotation = FQuat(HitResultReverseRotator);
				ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

				StartLocation = FirstHitResult.ImpactPoint + ForwardVector * 2.0f;
				StartLocation.Z += j * 5;

				EndLocation = StartLocation;
				EndLocation.Z -= 50 + j * 5;

				FHitResult SecondHitResult;
				PerformSphereTraceByChannel(Character->GetWorld(), SecondHitResult, StartLocation, EndLocation, 2.5f, ECC_Visibility, bDrawDebug);


				if (!SecondHitResult.bStartPenetrating)
				{
					if (SecondHitResult.bBlockingHit)
					{

						float StyleMultiplier = ClimbStyle.GetTagName().IsEqual("Parkour.ClimbStyle.Braced") ? CharacterHandFront : 0.0f;

						FVector LeftHandLedgeLocation = FirstHitResult.ImpactPoint + ForwardVector * (-3.0f + StyleMultiplier);;
						LeftHandLedgeLocation.Z = SecondHitResult.ImpactPoint.Z + CharacterHeightDiff + CharacterHandUp - 9.0f;

						FRotator LeftHandLedgeRotation = HitResultReverseRotator;
						LeftHandLedgeRotation.Roll -= 80.0f;
						LeftHandLedgeRotation.Pitch += 90.0f;

						if (!AnimInstance->GetClass()->ImplementsInterface(UParkourABPInterface::StaticClass()))
						{
							UE_LOG(LogTemp, Warning, TEXT("LeftHandLedgeIK: AnimInstance does not implement the ABP interface"));
							return;
						}

						IParkourABPInterface* ParkourABPInterface = Cast<IParkourABPInterface>(AnimInstance);
						ParkourABPInterface->Execute_SetLeftHandLedgeLocation(AnimInstance, LeftHandLedgeLocation);
						ParkourABPInterface->Execute_SetLeftHandLedgeRotation(AnimInstance, LeftHandLedgeRotation);

						bShouldBreak = true;
					}
					break;
				}
				else
					if (i == 5)
					{
						FVector LeftHandLedgeLocation = FirstHitResult.ImpactPoint;
						LeftHandLedgeLocation.Z -= 9.0f;

						FRotator LeftHandLedgeRotation = UParkourFunctionLibrary::NormalReverseRotationZ(FirstHitResult.ImpactNormal);
						LeftHandLedgeRotation.Roll -= 80.0f;
						LeftHandLedgeRotation.Pitch += 90.0f;

						if (!AnimInstance->GetClass()->ImplementsInterface(UParkourABPInterface::StaticClass()))
						{
							UE_LOG(LogTemp, Warning, TEXT("LeftHandLedgeIK: AnimInstance does not implement the ABP interface"));
							return;
						}

						IParkourABPInterface* ParkourABPInterface = Cast<IParkourABPInterface>(AnimInstance);
						ParkourABPInterface->Execute_SetLeftHandLedgeLocation(AnimInstance, LeftHandLedgeLocation);
						ParkourABPInterface->Execute_SetLeftHandLedgeRotation(AnimInstance, LeftHandLedgeRotation);

					}
			}
		}

		if (bShouldBreak)
			break;
	}

	//TODO fix when hand could not find location
	//solution copy another hand location
	if (!bShouldBreak)
	{
		UE_LOG(LogTemp, Warning, TEXT("IK WORKS"));
		FVector LeftHandLedgeLocation = CharacterMesh->GetSocketLocation("hand_l");

		if (!AnimInstance->GetClass()->ImplementsInterface(UParkourABPInterface::StaticClass()))
		{
			UE_LOG(LogTemp, Warning, TEXT("LeftHandLedgeIK: AnimInstance does not implement the ABP interface"));
			return;
		}

		IParkourABPInterface* ParkourABPInterface = Cast<IParkourABPInterface>(AnimInstance);
		ParkourABPInterface->Execute_SetLeftHandLedgeLocation(AnimInstance, LeftHandLedgeLocation);
	}
}

void UParkourComponent::LeftFootIK(FHitResult& LedgeResult)
{
	if (!ClimbStyle.GetTagName().IsEqual("Parkour.ClimbStyle.Braced"))
		return;

	for (int32 i = 0; i < 3; i++)
	{
		FQuat QuatRotation = FQuat(WallRotation);
		FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);
		FVector RightVector = QuatRotation.RotateVector(FVector::RightVector);

		FVector StartLocation = LedgeResult.ImpactPoint - (RightVector * 7.0f) + (ForwardVector * -30.0f);
		StartLocation.Z += i * 5.0f + (CharacterHeightDiff - 150.0f);
		FVector EndLocation = StartLocation + (ForwardVector * 60.0f);


		FHitResult HitResult;
		PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 6.0f, ECC_Visibility, bDrawDebug);

		if (HitResult.bBlockingHit)
		{
			QuatRotation = FQuat(UParkourFunctionLibrary::NormalReverseRotationZ(HitResult.ImpactNormal));
			ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

			FVector LeftFootLocation = HitResult.ImpactPoint + (ForwardVector * -17.0f);

			if (!AnimInstance->GetClass()->ImplementsInterface(UParkourABPInterface::StaticClass()))
			{
				UE_LOG(LogTemp, Warning, TEXT("LeftFootIK: AnimInstance does not implement the ABP interface"));
				return;
			}

			IParkourABPInterface* ParkourABPInterface = Cast<IParkourABPInterface>(AnimInstance);
			ParkourABPInterface->Execute_SetLeftFootLocation(AnimInstance, LeftFootLocation);
			break;
		}
		else
			if (i == 2)
			{
				for (int32 j = 0; j < 5; j++)
				{
					StartLocation = LedgeResult.ImpactPoint + (ForwardVector * -30.0f);
					StartLocation.Z += j * 5.0f + (CharacterHeightDiff - 150.0f);
					EndLocation = StartLocation + (ForwardVector * 60.0f);


					PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 6.0f, ECC_Visibility, bDrawDebug);

					if (HitResult.bBlockingHit)
					{
						QuatRotation = FQuat(UParkourFunctionLibrary::NormalReverseRotationZ(HitResult.ImpactNormal));
						ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

						FVector LeftFootLocation = HitResult.ImpactPoint + (ForwardVector * -17.0f);

						if (!AnimInstance->GetClass()->ImplementsInterface(UParkourABPInterface::StaticClass()))
						{
							UE_LOG(LogTemp, Warning, TEXT("LeftFootIK: AnimInstance does not implement the ABP interface"));
							return;
						}

						IParkourABPInterface* ParkourABPInterface = Cast<IParkourABPInterface>(AnimInstance);
						ParkourABPInterface->Execute_SetLeftFootLocation(AnimInstance, LeftFootLocation);
						break;
					}
				}
			}
	}
}

void UParkourComponent::RightClimbIK()
{
	FHitResult LedgeResult = SecondClimbLedgeResult;
	if (ParkourStateTag.GetTagName().IsEqual("Parkour.State.ReachLedge"))
	{
		if (LedgeResult.bBlockingHit)
		{
			RightHandLedgeIK(LedgeResult);

			RightFootIK(LedgeResult);
		}
	}
	//TODO find why this is even needed!
	LastClimbRightHandLocation;
}

void UParkourComponent::RightHandLedgeIK(FHitResult& LedgeResult)
{
	bool bShouldBreak = false;
	for (int32 i = 0; i < 5; i++)
	{

		FQuat QuatRotation = FQuat(WallRotation);
		FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);
		FVector RightVector = QuatRotation.RotateVector(FVector::RightVector);


		FVector StartLocation = LedgeResult.ImpactPoint - ForwardVector * 20.0f - RightVector * (-8.0f + i * -2.0f);
		FVector EndLocation = StartLocation + ForwardVector * 40;

		FHitResult FirstHitResult;
		PerformSphereTraceByChannel(Character->GetWorld(), FirstHitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug);

		if (FirstHitResult.bBlockingHit)
		{
			for (int32 j = 0; j < 6; j++)
			{
				FRotator HitResultReverseRotator = UParkourFunctionLibrary::NormalReverseRotationZ(FirstHitResult.ImpactNormal);
				QuatRotation = FQuat(HitResultReverseRotator);
				ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

				StartLocation = FirstHitResult.ImpactPoint + ForwardVector * 2.0f;
				StartLocation.Z += j * 5;

				EndLocation = StartLocation;
				EndLocation.Z -= 50 + j * 5;

				FHitResult SecondHitResult;
				PerformSphereTraceByChannel(Character->GetWorld(), SecondHitResult, StartLocation, EndLocation, 2.5f, ECC_Visibility, bDrawDebug);


				if (!SecondHitResult.bStartPenetrating)
				{
					if (SecondHitResult.bBlockingHit)
					{

						float StyleMultiplier = ClimbStyle.GetTagName().IsEqual("Parkour.ClimbStyle.Braced") ? CharacterHandFront : 0.0f;

						FVector RightHandLedgeLocation = FirstHitResult.ImpactPoint + ForwardVector * (-3.0f + StyleMultiplier);;
						RightHandLedgeLocation.Z = SecondHitResult.ImpactPoint.Z + CharacterHeightDiff + CharacterHandUp - 9.0f;

						FRotator RightHandLedgeRotation = FRotator(HitResultReverseRotator);
						RightHandLedgeRotation.Roll -= 90.0f;
						RightHandLedgeRotation.Pitch -= 90.0f;

						if (!AnimInstance->GetClass()->ImplementsInterface(UParkourABPInterface::StaticClass()))
						{
							UE_LOG(LogTemp, Warning, TEXT("RightHandLedgeIK: AnimInstance does not implement the ABP interface"));
							return;
						}

						IParkourABPInterface* ParkourABPInterface = Cast<IParkourABPInterface>(AnimInstance);
						ParkourABPInterface->Execute_SetRightHandLedgeLocation(AnimInstance, RightHandLedgeLocation);
						ParkourABPInterface->Execute_SetRightHandLedgeRotation(AnimInstance, RightHandLedgeRotation);

						bShouldBreak = true;
					}
					break;
				}
				else
					if (i == 5)
					{
						FVector RightHandLedgeLocation = FirstHitResult.ImpactPoint;
						RightHandLedgeLocation.Z -= 9.0f;

						FRotator RightHandLedgeRotation = UParkourFunctionLibrary::NormalReverseRotationZ(FirstHitResult.ImpactNormal);
						RightHandLedgeRotation.Roll -= 90.0f;
						RightHandLedgeRotation.Pitch -= 90.0f;

						if (!AnimInstance->GetClass()->ImplementsInterface(UParkourABPInterface::StaticClass()))
						{
							UE_LOG(LogTemp, Warning, TEXT("RightHandLedgeIK: AnimInstance does not implement the ABP interface"));
							return;
						}

						IParkourABPInterface* ParkourABPInterface = Cast<IParkourABPInterface>(AnimInstance);
						ParkourABPInterface->Execute_SetRightHandLedgeLocation(AnimInstance, RightHandLedgeLocation);
						ParkourABPInterface->Execute_SetRightHandLedgeRotation(AnimInstance, RightHandLedgeRotation);

					}
			}
		}

		if (bShouldBreak)
			break;
	}
}

void UParkourComponent::RightFootIK(FHitResult& LedgeResult)
{
	if (!ClimbStyle.GetTagName().IsEqual("Parkour.ClimbStyle.Braced"))
		return;

	for (int32 i = 0; i < 3; i++)
	{
		FQuat QuatRotation = FQuat(WallRotation);
		FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);
		FVector RightVector = QuatRotation.RotateVector(FVector::RightVector);

		FVector StartLocation = LedgeResult.ImpactPoint + (RightVector * 9.0f) + (ForwardVector * -30.0f);
		StartLocation.Z += i * 5.0f + (CharacterHeightDiff - 140.0f);
		FVector EndLocation = StartLocation + (ForwardVector * 60.0f);


		FHitResult HitResult;
		PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 6.0f, ECC_Visibility, bDrawDebug);

		if (HitResult.bBlockingHit)
		{
			QuatRotation = FQuat(UParkourFunctionLibrary::NormalReverseRotationZ(HitResult.ImpactNormal));
			ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

			FVector RightFootLocation = HitResult.ImpactPoint + (ForwardVector * -17.0f);

			if (!AnimInstance->GetClass()->ImplementsInterface(UParkourABPInterface::StaticClass()))
			{
				UE_LOG(LogTemp, Warning, TEXT("RightFootIK: AnimInstance does not implement the ABP interface"));
				return;
			}

			IParkourABPInterface* ParkourABPInterface = Cast<IParkourABPInterface>(AnimInstance);
			ParkourABPInterface->Execute_SetRightFootLocation(AnimInstance, RightFootLocation);
			break;
		}
		else
			if (i == 2)
			{
				for (int32 j = 0; j < 5; j++)
				{
					StartLocation = LedgeResult.ImpactPoint + (ForwardVector * -30.0f);
					StartLocation.Z += j * 5.0f + (CharacterHeightDiff - 140.0f);
					EndLocation = StartLocation + (ForwardVector * 60.0f);


					PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 6.0f, ECC_Visibility, bDrawDebug);

					if (HitResult.bBlockingHit)
					{
						QuatRotation = FQuat(UParkourFunctionLibrary::NormalReverseRotationZ(HitResult.ImpactNormal));
						ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

						FVector RightFootLocation = HitResult.ImpactPoint + (ForwardVector * -17.0f);

						if (!AnimInstance->GetClass()->ImplementsInterface(UParkourABPInterface::StaticClass()))
						{
							UE_LOG(LogTemp, Warning, TEXT("RightFootIK: AnimInstance does not implement the ABP interface"));
							return;
						}

						IParkourABPInterface* ParkourABPInterface = Cast<IParkourABPInterface>(AnimInstance);
						ParkourABPInterface->Execute_SetRightFootLocation(AnimInstance, RightFootLocation);
						break;
					}
				}
			}
	}
}

void UParkourComponent::PlayParkourMontage()
{
	SetParkourState(ParkourVariables->ParkourInState);

	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation("Parkour1",
		FindWarpLocation(WallTopResult.ImpactPoint, ParkourVariables->Warp1XOffset, ParkourVariables->Warp1ZOffset), WallRotation);

	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation("Parkour2",
		FindWarpLocation(WallDepthResult.ImpactPoint, ParkourVariables->Warp2XOffset, ParkourVariables->Warp2ZOffset), WallRotation);

	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation("Parkour3",
		FindWarpLocation(WallVaultResult.ImpactPoint, ParkourVariables->Warp3XOffset, ParkourVariables->Warp3ZOffset), WallRotation);

	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation("Parkour4",
		FindWarpLocationChecked(WallTopResult.ImpactPoint, ParkourVariables->Warp2XOffset, ParkourVariables->Warp2ZOffset), WallRotation);

	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation("Parkour5",
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

void UParkourComponent::FindMontageStartTime()
{
	if ((ParkourActionTag.GetTagName().IsEqual("Parkour.Action.Climb") || ParkourActionTag.GetTagName().IsEqual("Parkour.Action.FreeHangClimb")) && !bOnGround)
	{
		MontageStartTime = ParkourVariables->FallingMontageStartPosition;
		return;
	}

	MontageStartTime = ParkourVariables->MontageStartPosition;
}

void UParkourComponent::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Character->GetController() == nullptr)
		return;

	if (ParkourStateTag.GetTagName().IsEqual("Parkour.State.ReachLedge"))
		bFirstClimbMove = false;

	ForwardScale = MovementVector.Y;
	RightScale = MovementVector.X;

	GetClimbForwardValue(ForwardScale, HorizontalClimbForwardValue, VerticalClimbForwardValue);
	GetClimbRightValue(RightScale, HorizontalClimbRightValue, VerticalClimbRightValue);

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
		if (AnimInstance->IsAnyMontagePlaying())
			StopClimbMovement();
		else
			ClimbMove();
	}
}

void UParkourComponent::OnParkourMontageBlendOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
		return;

	//TODO There should be ParkourVariables pointer
	SetParkourState(BlendOutState);
	SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.NoAction")));
}

FVector UParkourComponent::FindWarpLocation(const FVector& ImpactPoint, float XOffset, float ZOffset) const
{
	FVector Result = ImpactPoint;
	Result.Z += ZOffset;

	FQuat QuatRotation = FQuat(WallRotation);
	Result += QuatRotation.RotateVector(FVector::ForwardVector) * XOffset;

	return Result;
}

FVector UParkourComponent::FindWarpLocationChecked(const FVector& ImpactPoint, float XOffset, float ZOffset) const
{
	FVector StartLocation = ImpactPoint + WallRotation.RotateVector(FVector::ForwardVector) * XOffset + FVector(0.0f, 0.0f, 40.0f);
	FVector EndLocation = StartLocation - FVector(0.0f, 0.0f, 60.0f);

	FHitResult HitResult;
	PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 25.0f, ECC_Visibility, bDrawDebug);

	FVector AdjustedImpactPoint = HitResult.bBlockingHit ? HitResult.ImpactPoint : ImpactPoint;

	AdjustedImpactPoint.Z += ZOffset;

	return AdjustedImpactPoint;
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

float UParkourComponent::FirstTraceHeight() const
{
	if (!ParkourStateTag.GetTagName().IsEqual("Parkour.State.Climb"))
		return -60.0f;

	float ClimbHeight = 0.0f;

	for (int32 i = 0; i < 5; i++)
	{
		FVector StartLocation = Character->GetActorLocation();
		float RightHandZLocation = CharacterMesh->GetSocketLocation("hand_r").Z;
		float LeftHandZLocation = CharacterMesh->GetSocketLocation("hand_l").Z;
		StartLocation.Z = RightHandZLocation < LeftHandZLocation ? LeftHandZLocation : RightHandZLocation;
		StartLocation.Z -= CharacterHeightDiff - CharacterHandUp;
		StartLocation -= Character->GetActorForwardVector() * 20.0f;

		FVector EndLocation = StartLocation + Character->GetActorForwardVector() * 20.0f * (i + 1);

		FHitResult FirstHitResult;
		PerformSphereTraceByChannel(Character->GetWorld(), FirstHitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug);

		if (!FirstHitResult.bBlockingHit)
			continue;

		for (int32 j = 0; j < 10; j++)
		{
			FQuat QuatRotation = FQuat(UParkourFunctionLibrary::NormalReverseRotationZ(FirstHitResult.ImpactNormal));
			FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);
			StartLocation = FirstHitResult.ImpactPoint + ForwardVector * 2.0f;
			StartLocation.Z += 5.0f + j * 10.0f;

			EndLocation = StartLocation;
			EndLocation.Z -= 25.0f + j * -5.0f;

			FHitResult SecondHitResult;
			PerformSphereTraceByChannel(Character->GetWorld(), SecondHitResult, StartLocation, EndLocation, 2.5f, ECC_Visibility, bDrawDebug);

			if (SecondHitResult.bBlockingHit && !SecondHitResult.bStartPenetrating)
			{
				ClimbHeight = SecondHitResult.ImpactPoint.Z;
				break;
			}
		}

		break;
	}

	return ClimbHeight - Character->GetActorLocation().Z - 4.0f;
}

void UParkourComponent::CheckClimbOrHop()
{
	FName DesireRotationName = GetClimbDesireRotation().GetTagName();

	if (DesireRotationName.IsEqual("Parkour.Direction.Forward") ||
		DesireRotationName.IsEqual("Parkour.Direction.ForwardLeft") ||
		DesireRotationName.IsEqual("Parkour.Direction.ForwardRight"))
	{
		if (!CheckMantleSurface())
		{
			FirstClimbLedgeResultCalculation();
			FindHopLocation();
			if (CheckLedgeValid())
			{
				//TODO extract from it method
				CheckClimbStyle();
				SecondClimbLedgeResultCalculation();
				SetParkourAction(SelectHopAction());
			}
			return;
		}

		if (ClimbStyle.GetTagName().IsEqual("Parkour.ClimbStyle.Braced"))
			SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.ClimbingUp")));
		else
			SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.FreeHangClimbUp")));
	}
	else
	{
		FirstClimbLedgeResultCalculation();
		FindHopLocation();
		if (CheckLedgeValid())
		{
			CheckClimbStyle();
			SecondClimbLedgeResultCalculation();
			SetParkourAction(SelectHopAction());
		}
	}
}

bool UParkourComponent::CheckAirHang() const
{
	if (!SecondClimbLedgeResult.bBlockingHit)
		return false;

	float HeadLocationZ = CharacterMesh->GetSocketLocation("head").Z;

	float LedgeLocationZ = SecondClimbLedgeResult.ImpactPoint.Z;

	if (bOnGround || HeadLocationZ - LedgeLocationZ <= 30.0f)
		return false;

	return true;
}

FRotator UParkourComponent::GetDesireRotation() const
{
	FRotator ControlRot = Character->GetControlRotation();
	ControlRot.Roll = 0.0f;
	ControlRot.Pitch = 0.0f;

	FQuat QuatRotation = FQuat(ControlRot);
	FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);
	FVector RightVector = QuatRotation.RotateVector(FVector::RightVector);

	FVector DirectionVector = ForwardVector * ForwardScale + RightVector * RightScale;
	DirectionVector.Normalize();

	FMatrix RotationMatrix = FRotationMatrix::MakeFromX(DirectionVector.Rotation().Vector());
	FRotator RotatorFromX = RotationMatrix.Rotator();
	RotatorFromX.Normalize();


	if (ForwardScale == 0 && RightScale == 0)
		return Character->GetActorRotation();
	else
		return RotatorFromX;

}

void UParkourComponent::FindDropDownHangLocation()
{
	FVector StartLocation = Character->GetActorLocation();
	FVector EndLocation = StartLocation - FVector(0.0f, 0.0f, 120.0f);

	FHitResult HitResult;
	PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 35.0f, ECC_Visibility, bDrawDebug);

	if (!HitResult.bBlockingHit || HitResult.bStartPenetrating)
		return;

	FQuat QuatRotation = FQuat(GetDesireRotation());
	FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

	StartLocation = HitResult.ImpactPoint + ForwardVector * 100.0f;
	StartLocation.Z -= 5.0f;

	EndLocation = StartLocation - ForwardVector * 125.0f;

	PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug);

	if (!HitResult.bBlockingHit || HitResult.bStartPenetrating)
		return;

	WallHitTraces.Empty();

	QuatRotation = FQuat(UParkourFunctionLibrary::NormalReverseRotationZ(HitResult.ImpactNormal));
	FVector RightVector = QuatRotation.RotateVector(FVector::RightVector);
	ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

	for (int32 i = 0; i < 5; i++)
	{
		StartLocation = HitResult.ImpactPoint + RightVector * 20 * (i - 2) - ForwardVector * 40.0f;
		EndLocation = StartLocation + ForwardVector * 70.0f;

		FHitResult LoopTraceHitResult;
		PerformLineTraceByChannel(Character->GetWorld(), LoopTraceHitResult, StartLocation, EndLocation, ECC_Visibility, bDrawDebug);

		HopHitTraces.Empty();

		HopHitTraces.Add(LoopTraceHitResult);

		for (int32 j = 0; j < 12; j++)
		{
			StartLocation.Z += 8.0f;
			EndLocation.Z += 8.0f;

			FHitResult InnerLoopTraceHitResult;
			PerformLineTraceByChannel(Character->GetWorld(), InnerLoopTraceHitResult, StartLocation, EndLocation, ECC_Visibility, bDrawDebug);

			HopHitTraces.Add(InnerLoopTraceHitResult);
		}

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

	if (WallHitTraces.Num() > 0)
		WallHitResult = WallHitTraces[0];

	for (int32 i = 1; i < WallHitTraces.Num(); i++)
	{
		float Distance1 = FVector::Distance(Character->GetActorLocation(), WallHitTraces[i].ImpactPoint);
		float Distance2 = FVector::Distance(Character->GetActorLocation(), WallHitResult.ImpactPoint);
		if (Distance1 <= Distance2)
		{
			WallHitResult = WallHitTraces[i];
		}
	}

	if (!WallHitResult.bBlockingHit || WallHitResult.bStartPenetrating)
		return;

	WallRotation = UParkourFunctionLibrary::NormalReverseRotationZ(WallHitResult.ImpactNormal);
	QuatRotation = FQuat(WallRotation);
	ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

	StartLocation = WallHitResult.ImpactPoint + (ForwardVector * 2.0f);
	StartLocation.Z += 7.0f;
	EndLocation = StartLocation;
	EndLocation.Z -= 7.0f;

	PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 2.5f, ECC_Visibility, bDrawDebug);

	if (HitResult.bBlockingHit)
	{
		WallTopResult = HitResult;
		if (CheckClimbSurface())
		{
			CheckClimbStyle();
			SecondClimbLedgeResultCalculation();

			if (ClimbStyle.GetTagName().IsEqual("Parkour.ClimbStyle.Braced"))
				SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.DropDown")));
			else
				SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.FreeHangDropDown")));
		}
		else
			ResetParkourResults();
	}
}

void UParkourComponent::FindHopLocation()
{

	float VerticalDirectionMultiplier = UParkourFunctionLibrary::SelectDirectionFloat(1.0f, -7.5f, -2.5f, -2.5f, -1.0f, -4.0f, -1.0f, -4.0f, GetClimbDesireRotation());
	float HorizontalDirectionMultiplier = UParkourFunctionLibrary::SelectDirectionFloat(0.0f, 0.0f, -1.0f, 1.0f, -0.75f, -0.75f, 0.75f, 0.75f, GetClimbDesireRotation());
	VerticalHopDistance = 25.0f * VerticalDirectionMultiplier;
	HorizontalHopDistance = 140.0f * HorizontalDirectionMultiplier;

	WallHitTraces.Empty();

	for (int32 i = 0; i < 7; i++)
	{
		FVector CharacterUpVector = Character->GetActorQuat().RotateVector(FVector::UpVector);
		FVector CharacterRightVector = Character->GetActorQuat().RotateVector(FVector::RightVector);

		FQuat WallQuatRotation = FQuat(WallRotation);
		FVector WallRightVector = WallQuatRotation.RotateVector(FVector::RightVector);
		FVector WallForwardVector = WallQuatRotation.RotateVector(FVector::ForwardVector);

		FVector StartLocation = WallTopResult.ImpactPoint;
		StartLocation += CharacterUpVector * VerticalHopDistance;
		StartLocation += CharacterRightVector * HorizontalHopDistance;
		StartLocation += WallRightVector * 20.0f * (i - 3);
		StartLocation -= WallForwardVector * 60.0f;

		FVector EndLocation = StartLocation + WallForwardVector * 120.0f;

		FHitResult OuterLoopTraceResult;
		PerformLineTraceByChannel(Character->GetWorld(), OuterLoopTraceResult, StartLocation, EndLocation, ECC_Visibility, bDrawDebug);

		HopHitTraces.Empty();

		StartLocation.Z -= 16.0f;
		EndLocation.Z -= 16.0f;

		for (int32 k = 0; k <= 20; k++)
		{
			StartLocation.Z += 8.0f;
			EndLocation.Z += 8.0f;

			FHitResult InnerLoopTraceResult;
			PerformLineTraceByChannel(Character->GetWorld(), InnerLoopTraceResult, StartLocation, EndLocation, ECC_Visibility, bDrawDebug);

			if (InnerLoopTraceResult.bStartPenetrating)
				continue;

			HopHitTraces.Add(InnerLoopTraceResult);
		}

		for (int32 j = 1; j < HopHitTraces.Num(); j++)
		{
			float Distance1 = HopHitTraces[j].bBlockingHit ? HopHitTraces[j].Distance : FVector::Distance(HopHitTraces[j].TraceStart, HopHitTraces[j].TraceEnd);
			float Distance2 = HopHitTraces[j - 1].bBlockingHit ? HopHitTraces[j - 1].Distance : FVector::Distance(HopHitTraces[j - 1].TraceStart, HopHitTraces[j - 1].TraceEnd);

			if (Distance1 - Distance2 <= 5.0f)
				continue;

			FQuat QuatRotation = FQuat(UParkourFunctionLibrary::NormalReverseRotationZ(HopHitTraces[j - 1].ImpactNormal));
			FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);
			FVector RightVector = QuatRotation.RotateVector(FVector::RightVector);

			float CapsuleHalfHeight = CapsuleComponent->GetUnscaledCapsuleHalfHeight() - 20.0f;

			FVector CapsuleStartLocation = HopHitTraces[j - 1].ImpactPoint - ForwardVector * 40.0f + RightVector * 4.0f;
			CapsuleStartLocation.Z -= CapsuleHalfHeight;

			FVector CapsuleEndLocation = CapsuleStartLocation - RightVector * 8.0f;

			FHitResult CapsuleCheckTrace;
			PerformCapsuleTraceByChannel(Character->GetWorld(), CapsuleCheckTrace, CapsuleStartLocation, CapsuleEndLocation, CapsuleHalfHeight, 25.0f, ECC_Visibility, bDrawDebug);


			if (!CapsuleCheckTrace.bBlockingHit)
				WallHitTraces.Add(HopHitTraces[j - 1]);

			break;

		}
	}

	if (WallHitTraces.Num() <= 0)
	{
		if (GetClimbDesireRotation().GetTagName().IsEqual("Parkour.Direction.Backward"))
			ParkourDrop();
		else if (bool bIsOutCorner = OutCornerHop() || CheckCornerHop())
		{
			WallRotation = CornerHopRotation;
			CornerHop(bIsOutCorner);
		}

		return;
	}

	WallHitResult = WallHitTraces[0];

	for (int32 i = 1; i < WallHitTraces.Num(); i++)
	{
		float Distance1 = FVector::Distance(Character->GetActorLocation(), WallHitTraces[i].ImpactPoint);
		float Distance2 = FVector::Distance(Character->GetActorLocation(), WallHitResult.ImpactPoint);
		if (Distance1 <= Distance2)
			WallHitResult = WallHitTraces[i];
	}

	if (WallHitResult.bStartPenetrating)
		return;

	if (!ParkourStateTag.GetTagName().IsEqual("Parkour.State.Climb"))
		WallRotation = UParkourFunctionLibrary::NormalReverseRotationZ(WallHitResult.ImpactNormal);

	FVector StartLocation = WallHitResult.ImpactPoint;
	StartLocation.Z += 3.0f;
	FVector EndLocation = StartLocation;
	EndLocation.Z -= 3.0f;

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
	DesireRotationZ = GetVerticalAxis();
	DesireRotationY = GetHorizontalAxis();

	if (DesireRotationZ >= 0.5f && DesireRotationZ <= 1.0f) {
		if (DesireRotationY >= -0.5f && DesireRotationY <= 0.0f)
			return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.Forward"));
		if (DesireRotationY >= 0.5f && DesireRotationY <= 1.0f)
			return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.ForwardRight"));
		if (DesireRotationY >= -1.0f && DesireRotationY <= -0.5f)
			return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.ForwardLeft"));
	}

	if (DesireRotationZ >= -0.5f && DesireRotationZ <= 0.5f) {
		if (DesireRotationY >= 0.5f && DesireRotationY <= 1.0f)
			return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.Right"));
		if (DesireRotationY >= -1.0f && DesireRotationY <= -0.5f)
			return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.Left"));
	}

	if (DesireRotationZ >= -1.0f && DesireRotationZ <= -0.5f) {
		if (DesireRotationY >= -0.5f && DesireRotationY <= 0.5f)
			return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.Backward"));
		if (DesireRotationY >= 0.5f && DesireRotationY <= 1.0f)
			return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.BackwardRight"));
		if (DesireRotationY >= -1.0f && DesireRotationY <= -0.5f)
			return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.BackwardLeft"));
	}

	return UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.Forward"));
}

void UParkourComponent::SetClimbDirection(const FGameplayTag& NewClimbDirection)
{
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

void UParkourComponent::StopClimbMovement()
{
	CharacterMovement->StopMovementImmediately();
	SetClimbDirection(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.NoDirection")));
}

void UParkourComponent::ResetMovement()
{
	ForwardScale = 0.0f;
	RightScale = 0.0f;
	SetClimbDirection(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Direction.NoDirection")));
}

void UParkourComponent::ClimbMove()
{
	if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.CornerMove"))
		return;

	if (FMath::Abs(GetHorizontalAxis()) <= 0.7f)
	{
		StopClimbMovement();
		return;
	}

	FName NewDirectionString = GetHorizontalAxis() > 0.0f ? "Parkour.Direction.Right" : "Parkour.Direction.Left";

	SetClimbDirection(UGameplayTagsManager::Get().RequestGameplayTag(FName(NewDirectionString)));

	bool bShouldBreak = false;
	FHitResult OuterLoopHitResult;
	FHitResult InnerLoopHitResult;
	FVector StartLocation;
	FVector EndLocation;
	FQuat QuatRotation;

	for (int32 i = 0; i < 3; i++)
	{
		QuatRotation = FQuat(ArrowActor->GetArrowComponent()->GetComponentRotation());
		FVector RightVector = QuatRotation.RotateVector(FVector::RightVector);
		FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

		StartLocation = ArrowActor->GetArrowComponent()->GetComponentLocation() + (ClimbMoveCheckDistance * GetHorizontalAxis() * RightVector);
		StartLocation.Z -= i * 10.0f;
		EndLocation = StartLocation + ForwardVector * 60.0f;

		OuterLoopHitResult;
		PerformSphereTraceByChannel(Character->GetWorld(), OuterLoopHitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug);

		if (OuterLoopHitResult.bStartPenetrating)
			continue;

		int32 OutCornerIndex;
		if (CheckOutCorner(OutCornerIndex))
		{
			OutCornerMove(OutCornerIndex);
			break;
		}

		if (!OuterLoopHitResult.bBlockingHit)
		{
			// iterates till i is not the last one
			if (i != 2)
				continue;

			CheckInCorner();
			break;
		}

		for (int32 j = 0; j < 7; j++)
		{
			QuatRotation = FQuat(UParkourFunctionLibrary::NormalReverseRotationZ(OuterLoopHitResult.ImpactNormal));
			ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

			StartLocation = OuterLoopHitResult.ImpactPoint + ForwardVector * 2.0f;
			StartLocation.Z += 5.0f * (j + 1);

			EndLocation = StartLocation;
			EndLocation.Z -= 55.0f;

			PerformSphereTraceByChannel(Character->GetWorld(), InnerLoopHitResult, StartLocation, EndLocation, 2.5f, ECC_Visibility, bDrawDebug);

			if (InnerLoopHitResult.bStartPenetrating)
			{
				if (i == 2 && j == 6)
				{
					StopClimbMovement();
					return;
				}

				continue;
			}

			if (InnerLoopHitResult.bBlockingHit)
				bShouldBreak = true;
			else
				StopClimbMovement();

			break;
		}

		if (bShouldBreak)
			break;
	}

	if (!bShouldBreak)
		return;

	for (int32 i = 0; i < 6; i++)
	{
		QuatRotation = FQuat(UParkourFunctionLibrary::NormalReverseRotationZ(OuterLoopHitResult.ImpactNormal));
		FVector RightVector = QuatRotation.RotateVector(FVector::RightVector);

		FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);
		float StyleMultiplier = UParkourFunctionLibrary::SelectClimbStyleFloat(-44.0f, -7.0f, ClimbStyle);

		ForwardVector* StyleMultiplier + OuterLoopHitResult.ImpactPoint;

		StartLocation = InnerLoopHitResult.ImpactPoint;

		StartLocation.Z += i * 5.0f + 2.0f;
		EndLocation = StartLocation + RightVector * GetHorizontalAxis() * 15.0f;
		FHitResult LineTraceHitResult;
		PerformLineTraceByChannel(Character->GetWorld(), LineTraceHitResult, StartLocation, EndLocation, ECC_Visibility, bDrawDebug);

		if (!LineTraceHitResult.bBlockingHit)
		{
			if (CheckClimbMoveSurface(OuterLoopHitResult))
				WallRotation = QuatRotation.Rotator();
			else
			{
				StopClimbMovement();
				return;
			}

			break;
		}
		else if (i == 5)
		{
			StopClimbMovement();
			return;
		}
	}

	QuatRotation = FQuat(UParkourFunctionLibrary::NormalReverseRotationZ(OuterLoopHitResult.ImpactNormal));
	FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);
	FVector NewCharacterLocation;
	FRotator NewCharacterRotation;

	float StyleMultiplier = UParkourFunctionLibrary::SelectClimbStyleFloat(-44.0f, -7.0f, ClimbStyle);
	FVector OuterLoopVector = OuterLoopHitResult.ImpactPoint + ForwardVector * StyleMultiplier;

	float StyleSub = UParkourFunctionLibrary::SelectClimbStyleFloat(107.0f, 115.0f, ClimbStyle);
	float TargetInterpZ = InnerLoopHitResult.ImpactPoint.Z + CharacterHeightDiff - StyleSub;

	float DeltaSeconds = Character->GetWorld()->GetDeltaSeconds();
	float StyleInterpSpeed = UParkourFunctionLibrary::SelectClimbStyleFloat(2.7f, 1.8f, ClimbStyle);
	NewCharacterLocation.X = FMath::FInterpTo(Character->GetActorLocation().X, OuterLoopVector.X, DeltaSeconds, GetClimbMoveSpeed());
	NewCharacterLocation.Y = FMath::FInterpTo(Character->GetActorLocation().Y, OuterLoopVector.Y, DeltaSeconds, GetClimbMoveSpeed());
	NewCharacterLocation.Z = FMath::FInterpTo(Character->GetActorLocation().Z, TargetInterpZ, DeltaSeconds, StyleInterpSpeed);

	NewCharacterRotation = FMath::RInterpTo(Character->GetActorRotation(), WallRotation, DeltaSeconds, 4.0f);
	Character->SetActorLocationAndRotation(NewCharacterLocation, NewCharacterRotation);

	SetClimbStyleOnMove(InnerLoopHitResult, WallRotation);

	bFirstClimbMove = true;
}

void UParkourComponent::CornerMove(const FVector& TargerRelativeLocation, const FRotator& TargerRelativeRotation)
{
	bFirstClimbMove = true;

	SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.CornerMove")));

	FName NewDirectionName = GetHorizontalAxis() > 0.0f ? "Parkour.Direction.Right" : "Parkour.Direction.Left";
	SetClimbDirection(UGameplayTagsManager::Get().RequestGameplayTag(FName(NewDirectionName)));

	float OverTimeByStyle = UParkourFunctionLibrary::SelectClimbStyleFloat(0.5f, 0.9f, ClimbStyle);

	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.Linkage = 0;
	LatentInfo.UUID = 1;
	LatentInfo.ExecutionFunction = FName("OnCornerMoveCompleted");

	UKismetSystemLibrary::MoveComponentTo(CapsuleComponent, TargerRelativeLocation, TargerRelativeRotation, false, false,
		OverTimeByStyle, true, EMoveComponentAction::Type::Move, LatentInfo);
}

void UParkourComponent::OnCornerMoveCompleted()
{
	SetParkourAction(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.NoAction")));
}

void UParkourComponent::OutCornerMove(const int32& OutCornerIndex)
{
	//TODO Rename hit results

	FVector StartLocation;
	FVector EndLocation;
	FHitResult HitResult;
	FHitResult LoopHitResult;

	FVector RightVector = ArrowActor->GetArrowComponent()->GetComponentQuat().RotateVector(FVector::RightVector);

	StartLocation = ArrowActor->GetArrowComponent()->GetComponentLocation();
	StartLocation.Z -= OutCornerIndex * 10.0f;
	EndLocation = StartLocation + RightVector * (GetHorizontalAxis() * 60.0f);

	PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug, 0.f);

	if (!HitResult.bBlockingHit)
		return;

	for (int32 i = 0; i < 5; i++)
	{
		StartLocation = HitResult.ImpactPoint;
		StartLocation.Z += 5.0f * (i + 1);

		EndLocation = StartLocation;
		EndLocation.Z -= 5.0f * (i + 10);

		PerformSphereTraceByChannel(Character->GetWorld(), LoopHitResult, StartLocation, EndLocation, 2.5f, ECC_Visibility, bDrawDebug, 0.f);

		if (!LoopHitResult.bStartPenetrating)
			break;

		if (i == 4)
		{
			StopClimbMovement();
			return;
		}
	}

	if (!LoopHitResult.bBlockingHit)
	{
		StopClimbMovement();
		return;
	}

	FRotator TargetRelativeRotation = UParkourFunctionLibrary::NormalReverseRotationZ(HitResult.ImpactNormal);
	FQuat QuatRotation = FQuat(TargetRelativeRotation);
	FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

	float StyleMultiplier = UParkourFunctionLibrary::SelectClimbStyleFloat(44.0f, 7.0f, ClimbStyle);
	FVector TargetRelativeLocation = HitResult.ImpactPoint - ForwardVector * StyleMultiplier;
	TargetRelativeLocation.Z = LoopHitResult.ImpactPoint.Z - 107.0f;

	CornerMove(TargetRelativeLocation, TargetRelativeRotation);
}

void UParkourComponent::CornerHop(bool bIsOutCorner)
{
	float CornerHorizontalHopDistance = bIsOutCorner ? 50.0f : 20.0f;
	CornerHorizontalHopDistance *= GetHorizontalAxis();

	int32 FirstIndex = GetHorizontalAxis() < 0.0f ? 0 : 4;
	int32 LastIndex = GetHorizontalAxis() < 0.0f ? 3 : 7;

	WallHitTraces.Empty();


	for (int32 i = FirstIndex; i < LastIndex; i++)
	{
		FVector CharacterUpVector = Character->GetActorQuat().RotateVector(FVector::UpVector);
		FVector CharacterRightVector = Character->GetActorQuat().RotateVector(FVector::RightVector);

		FQuat WallQuatRotation = FQuat(WallRotation);
		FVector WallRightVector = WallQuatRotation.RotateVector(FVector::RightVector);
		FVector WallForwardVector = WallQuatRotation.RotateVector(FVector::ForwardVector);

		FVector StartLocation = WallTopResult.ImpactPoint;
		StartLocation += CharacterUpVector * VerticalHopDistance;
		StartLocation += CharacterRightVector * CornerHorizontalHopDistance;
		StartLocation += WallRightVector * 20.0f * (i - 3);
		StartLocation -= WallForwardVector * 60.0f;

		FVector EndLocation = StartLocation + WallForwardVector * 120.0f;

		FHitResult OuterLoopTraceResult;
		PerformLineTraceByChannel(Character->GetWorld(), OuterLoopTraceResult, StartLocation, EndLocation, ECC_Visibility, bDrawDebug);

		HopHitTraces.Empty();

		StartLocation.Z -= 16.0f;
		EndLocation.Z -= 16.0f;

		for (int32 k = 0; k <= 20; k++)
		{
			StartLocation.Z += 8.0f;
			EndLocation.Z += 8.0f;

			FHitResult InnerLoopTraceResult;
			PerformLineTraceByChannel(Character->GetWorld(), InnerLoopTraceResult, StartLocation, EndLocation, ECC_Visibility, bDrawDebug);

			if (InnerLoopTraceResult.bStartPenetrating)
				continue;

			HopHitTraces.Add(InnerLoopTraceResult);
		}

		for (int32 j = 1; j < HopHitTraces.Num(); j++)
		{
			float Distance1 = HopHitTraces[j].bBlockingHit ? HopHitTraces[j].Distance : FVector::Distance(HopHitTraces[j].TraceStart, HopHitTraces[j].TraceEnd);
			float Distance2 = HopHitTraces[j - 1].bBlockingHit ? HopHitTraces[j - 1].Distance : FVector::Distance(HopHitTraces[j - 1].TraceStart, HopHitTraces[j - 1].TraceEnd);

			if (Distance1 - Distance2 <= 5.0f)
				continue;

			FQuat QuatRotation = FQuat(UParkourFunctionLibrary::NormalReverseRotationZ(HopHitTraces[j - 1].ImpactNormal));
			FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);
			FVector RightVector = QuatRotation.RotateVector(FVector::RightVector);

			float CapsuleHalfHeight = CapsuleComponent->GetUnscaledCapsuleHalfHeight() - 20.0f;

			FVector CapsuleStartLocation = HopHitTraces[j - 1].ImpactPoint - ForwardVector * 40.0f + RightVector * 4.0f;
			CapsuleStartLocation.Z -= CapsuleHalfHeight;

			FVector CapsuleEndLocation = CapsuleStartLocation - RightVector * 8.0f;

			FHitResult CapsuleCheckTrace;
			PerformCapsuleTraceByChannel(Character->GetWorld(), CapsuleCheckTrace, CapsuleStartLocation, CapsuleEndLocation, CapsuleHalfHeight, 25.0f, ECC_Visibility, bDrawDebug);


			if (!CapsuleCheckTrace.bBlockingHit)
				WallHitTraces.Add(HopHitTraces[j - 1]);

			break;

		}
	}

	if (!CheckLedgeValid())
		return;

	if (WallHitTraces.Num() <= 0)
		return;

	WallHitResult = WallHitTraces[0];

	for (int32 i = 1; i < WallHitTraces.Num(); i++)
	{
		float Distance1 = FVector::Distance(Character->GetActorLocation(), WallHitTraces[i].ImpactPoint);
		float Distance2 = FVector::Distance(Character->GetActorLocation(), WallHitResult.ImpactPoint);
		if (Distance1 <= Distance2)
			WallHitResult = WallHitTraces[i];
	}

	if (WallHitResult.bStartPenetrating)
		return;

	if (!ParkourStateTag.GetTagName().IsEqual("Parkour.State.Climb"))
		WallRotation = UParkourFunctionLibrary::NormalReverseRotationZ(WallHitResult.ImpactNormal);

	FVector StartLocation = WallHitResult.ImpactPoint;
	StartLocation.Z += 3.0f;
	FVector EndLocation = StartLocation;
	EndLocation.Z -= 3.0f;

	FHitResult HitResult;
	PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug);

	if (!HitResult.bBlockingHit)
		return;

	WallTopResult = HitResult;
}

bool UParkourComponent::OutCornerHop()
{
	FQuat ArrowQuatRotation = ArrowActor->GetArrowComponent()->GetComponentQuat();
	FVector ArrowRightVector = ArrowQuatRotation.RotateVector(FVector::RightVector);
	FVector ArrowForwardVector = ArrowQuatRotation.RotateVector(FVector::ForwardVector);

	for (int32 i = 0; i < 23; i++)
	{
		FVector StartLocation = ArrowActor->GetArrowComponent()->GetComponentLocation() - ArrowForwardVector * 20.0f;
		StartLocation.Z = Character->GetActorLocation().Z + i * 8.0f;
		StartLocation -= ArrowRightVector * GetHorizontalAxis() * 20.0f;
		FVector EndLocation = StartLocation + ArrowRightVector * GetHorizontalAxis() * 120.0f;

		FHitResult HitResult;
		PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug);

		if (HitResult.bBlockingHit && !HitResult.bStartPenetrating)
		{
			CornerHopRotation = UParkourFunctionLibrary::NormalReverseRotationZ(HitResult.ImpactNormal);
			return true;
		}
	}

	return false;
}

bool UParkourComponent::CheckClimbMoveSurface(const FHitResult& MovementHitResult) const
{
	FQuat QuatRotation = FQuat(ArrowActor->GetArrowComponent()->GetComponentRotation());
	FVector RightVector = QuatRotation.RotateVector(FVector::RightVector);
	FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

	FVector StartLocation = MovementHitResult.ImpactPoint + GetHorizontalAxis() * RightVector * 13.0f - ForwardVector * 40.0f;
	StartLocation.Z -= 90.0f;

	FVector EndLocation = StartLocation + ForwardVector * 15.0f;

	FHitResult HitResult;
	PerformCapsuleTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 82.0f, 5.0f, ECC_Visibility, bDrawDebug);

	return !HitResult.bBlockingHit;
}

bool UParkourComponent::CheckOutCorner(int32& OutCornerIndex) const
{
	bool OutCornerResult;
	OutCornerIndex = 0;
	for (int32 i = -2; i < 4; i++)
	{
		FVector RightVector = ArrowActor->GetArrowComponent()->GetComponentQuat().RotateVector(FVector::RightVector);
		FVector ForwardVector = ArrowActor->GetArrowComponent()->GetComponentQuat().RotateVector(FVector::ForwardVector);

		FVector StartLocation = ArrowActor->GetArrowComponent()->GetComponentLocation() + RightVector * (GetHorizontalAxis() * 35.0f);
		StartLocation.Z -= i * 10.0f;

		FVector EndLocation = StartLocation + ForwardVector * 100.0f;

		FHitResult HitResult;
		PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug, 0.0f);

		OutCornerResult = HitResult.bStartPenetrating;

		if (OutCornerResult)
		{
			OutCornerIndex = i;
			break;
		}
	}

	return OutCornerResult;
}

bool UParkourComponent::CheckInCorner()
{
	//TODO is this needed??
	bool bIsForwardDirection = GetClimbDesireRotation().GetTagName().IsEqual("Parkour.Direction.Forward");
	bool bIsBackwardDirection = GetClimbDesireRotation().GetTagName().IsEqual("Parkour.Direction.Backward");

	if (bIsForwardDirection || bIsBackwardDirection)
		return false;

	FQuat QuatArrowRotation = ArrowActor->GetArrowComponent()->GetComponentQuat();
	FVector ArrowRightVector = QuatArrowRotation.RotateVector(FVector::RightVector);
	FVector ArrowForwardVector = QuatArrowRotation.RotateVector(FVector::ForwardVector);
	FVector ArrowLocation = ArrowActor->GetArrowComponent()->GetComponentLocation();

	FHitResult LocalCornerDepth;
	FHitResult LocalTopResult;
	FHitResult HitResult;
	for (int32 i = 0; i < 6; i++)
	{
		FVector StartLocation = ArrowLocation + ArrowRightVector * GetHorizontalAxis() * i * 10.0f;
		StartLocation -= ArrowForwardVector * 10.0f;
		FVector EndLocation = StartLocation + ArrowForwardVector * 90.0f;

		PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug, 0.0f);

		if (!HitResult.bBlockingHit)
			break;

		if (i == 5)
		{
			StopClimbMovement();
			return false;
		}
		else
			LocalCornerDepth = HitResult;
	}

	for (int32 i = 0; i < 3; i++)
	{

		FVector StartLocation = LocalCornerDepth.ImpactPoint + ArrowForwardVector * 10.0f;
		StartLocation += ArrowRightVector * GetHorizontalAxis() * 20.0f;
		StartLocation.Z -= i * 10.0f;
		FVector EndLocation = StartLocation - ArrowRightVector * GetHorizontalAxis() * 70.0f;

		PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug, 0.0f);

		if (HitResult.bBlockingHit)
			break;

		if (i == 2)
		{
			StopClimbMovement();
			return false;
		}
	}

	for (int32 i = 0; i < 5; i++)
	{
		FQuat QuatRotation = FQuat(UParkourFunctionLibrary::NormalReverseRotationZ(HitResult.ImpactNormal));
		FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

		FVector StartLocation = HitResult.ImpactPoint + ForwardVector * 2.0f;
		StartLocation.Z += 5.0f * (i + 1);
		FVector EndLocation = StartLocation;
		EndLocation.Z -= 5.0f * (i + 10);

		PerformSphereTraceByChannel(Character->GetWorld(), LocalTopResult, StartLocation, EndLocation, 2.5f, ECC_Visibility, bDrawDebug, 0.0f);

		if (!LocalTopResult.bBlockingHit)
			break;
	}

	if (!LocalTopResult.bBlockingHit)
		return false;

	FQuat QuatRotation = FQuat(UParkourFunctionLibrary::NormalReverseRotationZ(HitResult.ImpactNormal));
	FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

	FVector StartLocation = LocalTopResult.ImpactPoint - ForwardVector * 50.0f;
	StartLocation.Z -= CapsuleComponent->GetUnscaledCapsuleHalfHeight();

	FHitResult CapsuleCheckResult;
	PerformCapsuleTraceByChannel(Character->GetWorld(), CapsuleCheckResult, StartLocation, StartLocation, 90.0f, 27.0f, ECC_Visibility, bDrawDebug);

	if (CapsuleCheckResult.bBlockingHit)
	{
		StopClimbMovement();
		return false;
	}

	WallRotation = QuatRotation.Rotator();

	float StyleMultiplier = UParkourFunctionLibrary::SelectClimbStyleFloat(44.0f, 9.0f, ClimbStyle);
	FVector TargetRelativeLocation = HitResult.ImpactPoint - ForwardVector * StyleMultiplier;
	TargetRelativeLocation.Z = LocalTopResult.ImpactPoint.Z;
	TargetRelativeLocation.Z += CharacterHeightDiff - UParkourFunctionLibrary::SelectClimbStyleFloat(107.0f, 118.0f, ClimbStyle);

	CornerMove(TargetRelativeLocation, WallRotation);

	return false;
}

bool UParkourComponent::CheckCornerHop()
{
	if (GetClimbDesireRotation().GetTagName().IsEqual("Parkour.Direction.Forward") ||
		GetClimbDesireRotation().GetTagName().IsEqual("Parkour.Direction.Backward"))
		return false;

	FQuat ArrowQuatRotation = ArrowActor->GetArrowComponent()->GetComponentQuat();
	FVector ArrowRightVector = ArrowQuatRotation.RotateVector(FVector::RightVector);
	FVector ArrowForwardVector = ArrowQuatRotation.RotateVector(FVector::ForwardVector);

	FHitResult LocalCornerDepth;
	for (int32 i = 0; i < 6; i++)
	{
		FVector StartLocation = ArrowActor->GetArrowComponent()->GetComponentLocation();
		StartLocation += ArrowRightVector * GetHorizontalAxis() * i * 10.0f;
		StartLocation -= ArrowForwardVector * 10.0f;
		FVector EndLocation = StartLocation + ArrowForwardVector * 90.0f;

		FHitResult HitResult;
		PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug);

		if (!HitResult.bBlockingHit)
			break;
		else if (i != 5)
			LocalCornerDepth = HitResult;
		else
			return false;
	}

	for (int32 i = 0; i < 23; i++)
	{
		FVector StartLocation = LocalCornerDepth.ImpactPoint + ArrowForwardVector * 10.0f;
		StartLocation.Z = Character->GetActorLocation().Z + i * 8.0f;
		StartLocation += ArrowRightVector * GetHorizontalAxis() * 10.0f;
		FVector EndLocation = StartLocation - ArrowRightVector * GetHorizontalAxis() * 60.0f;

		FHitResult HitResult;
		PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility, bDrawDebug);

		if (HitResult.bBlockingHit && !HitResult.bStartPenetrating)
		{
			CornerHopRotation = UParkourFunctionLibrary::NormalReverseRotationZ(HitResult.ImpactNormal);
			return true;
		}
		else if (i == 22)
			return false;
	}

	return false;
}

bool UParkourComponent::CheckLedgeValid()
{
	if (WallHitTraces.Num() <= 0)
	{
		ResetParkourResults();
		return false;
	}
	else
		return true;
}

float UParkourComponent::GetClimbMoveSpeed() const
{
	float BracedClampedSpeed = FMath::Clamp(AnimInstance->GetCurveValue("Climb Move Speed"), 1.0f, 98.0f);
	float FreeHangClampedSpeed = FMath::Clamp(AnimInstance->GetCurveValue("Climb Move Speed"), 1.0f, 55.0f);

	return UParkourFunctionLibrary::SelectClimbStyleFloat(BracedClampedSpeed * 0.1f, FreeHangClampedSpeed * 0.08f, ClimbStyle);
}

void UParkourComponent::SetClimbStyleOnMove(const FHitResult& HitResult, const FRotator& Rotation)
{
	FQuat QuatRotation = FQuat(Rotation);
	FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

	FVector StartLocation = HitResult.ImpactPoint - ForwardVector * 10.0f;
	StartLocation.Z -= 125.0f;
	FVector EndLocation = StartLocation + ForwardVector * 35.0f;

	FHitResult TraceResult;
	PerformSphereTraceByChannel(Character->GetWorld(), TraceResult, StartLocation, EndLocation, 10.0f, ECC_Visibility, bDrawDebug);

	if (TraceResult.bBlockingHit)
		SetClimbStyle(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.ClimbStyle.Braced")));
	else
		SetClimbStyle(UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.ClimbStyle.FreeHang")));
}

FGameplayTag UParkourComponent::SelectHopAction()
{
	FGameplayTag ForwardHopMovementTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Parkour.Action.ClimbHopUp"));

	if (ClimbStyle.GetTagName().IsEqual("Parkour.ClimbStyle.Braced"))
	{
		if (CheckCornerHop() || OutCornerHop())
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
		if (CheckCornerHop() || OutCornerHop())
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

void UParkourComponent::ClimbMoveIK()
{
	if (!ParkourStateTag.GetTagName().IsEqual("Parkour.State.Climb"))
		return;

	ClimbMoveHandIK();
	ClimbMoveFootIK();
}

void UParkourComponent::ClimbMoveHandIK()
{
	if (!ParkourStateTag.GetTagName().IsEqual("Parkour.State.Climb") || !bFirstClimbMove)
		return;

	ClimbMoveLeftHandIK();
	ClimbMoveRightHandIK();

}

void UParkourComponent::ClimbMoveLeftHandIK()
{
	FHitResult HitResult;

	for (int32 i = 0; i < 5; i++)
	{
		FVector InitialVector = CharacterMesh->GetSocketLocation("ik_hand_l") + Character->GetActorRightVector() * (i * 2.0f + ClimbHandSpace);
		InitialVector.Z -= CharacterHeightDiff;

		FVector StartLocation = InitialVector - Character->GetActorForwardVector() * 50.0f;
		FVector EndLocation = StartLocation + Character->GetActorForwardVector() * 120.0f;

		PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 15.0f, ECC_Visibility, bDrawDebug, 0.0f);

		if (HitResult.bStartPenetrating || !HitResult.bBlockingHit)
			continue;

		LeftHandClimbResult = HitResult;
		break;
	}

	for (int32 i = 0; i < 6; i++)
	{
		FVector StartLocation = LeftHandClimbResult.ImpactPoint;
		StartLocation.Z = ArrowActor->GetArrowComponent()->GetComponentLocation().Z + 10.0f + i * 5.0f;

		FVector EndLocation = StartLocation;
		EndLocation.Z -= 5 * (i + 10);

		PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 2.5f, ECC_Visibility, bDrawDebug, 0.0f);

		if (!HitResult.bStartPenetrating)
		{
			if (!HitResult.bBlockingHit)
				return;

			break;
		}
	}

	FQuat QuatRotation = FRotationMatrix::MakeFromX(LeftHandClimbResult.ImpactNormal).ToQuat();
	FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

	FVector LeftHandLocation = LeftHandClimbResult.ImpactPoint - ForwardVector * (UParkourFunctionLibrary::SelectClimbStyleFloat(CharacterHandFront, 0.0f, ClimbStyle) - 3.0f);
	LeftHandLocation.Z = HitResult.ImpactPoint.Z + CharacterHeightDiff + CharacterHandUp + GetClimbLeftHandZOffset();
	LeftHandLocation.Z -= UParkourFunctionLibrary::SelectClimbStyleFloat(9.0f, 8.0f, ClimbStyle);

	FRotator LeftHandRotation = FRotationMatrix::MakeFromX(LeftHandClimbResult.ImpactNormal).Rotator();
	UParkourFunctionLibrary::ReverseRotation(LeftHandRotation);
	LeftHandRotation.Roll += 280.0f;
	LeftHandRotation.Pitch += 90.0f;


	if (!AnimInstance->GetClass()->ImplementsInterface(UParkourABPInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("ClimbMoveLeftHandIK: AnimInstance does not implement the ABP interface"));
		return;
	}

	IParkourABPInterface* ParkourABPInterface = Cast<IParkourABPInterface>(AnimInstance);
	ParkourABPInterface->Execute_SetLeftHandLedgeLocation(AnimInstance, LeftHandLocation);
	ParkourABPInterface->Execute_SetLeftHandLedgeRotation(AnimInstance, LeftHandRotation);
}

void UParkourComponent::ClimbMoveRightHandIK()
{
	FHitResult HitResult;

	for (int32 i = 0; i < 5; i++)
	{
		FVector InitialVector = CharacterMesh->GetSocketLocation("ik_hand_r") - Character->GetActorRightVector() * (i * 2.0f + ClimbHandSpace);
		InitialVector.Z -= CharacterHeightDiff;

		FVector StartLocation = InitialVector - Character->GetActorForwardVector() * 50.0f;
		FVector EndLocation = StartLocation + Character->GetActorForwardVector() * 120.0f;

		PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 15.0f, ECC_Visibility, bDrawDebug, 0.0f);

		if (HitResult.bStartPenetrating || !HitResult.bBlockingHit)
			continue;

		RightHandClimbResult = HitResult;
		break;
	}

	for (int32 i = 0; i < 6; i++)
	{
		FVector StartLocation = RightHandClimbResult.ImpactPoint;
		StartLocation.Z = ArrowActor->GetArrowComponent()->GetComponentLocation().Z + 10.0f + i * 5.0f;

		FVector EndLocation = StartLocation;
		EndLocation.Z -= 5 * (i + 10);

		PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 2.5f, ECC_Visibility, bDrawDebug, 0.0f);

		if (!HitResult.bStartPenetrating)
		{
			if (!HitResult.bBlockingHit)
				return;

			break;
		}
	}

	FQuat QuatRotation = FRotationMatrix::MakeFromX(RightHandClimbResult.ImpactNormal).ToQuat();
	FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

	FVector RightHandLocation = RightHandClimbResult.ImpactPoint - ForwardVector * (UParkourFunctionLibrary::SelectClimbStyleFloat(CharacterHandFront, 0.0f, ClimbStyle) - 3.0f);
	RightHandLocation.Z = HitResult.ImpactPoint.Z + CharacterHeightDiff + CharacterHandUp + GetClimbRightHandZOffset();
	RightHandLocation.Z -= UParkourFunctionLibrary::SelectClimbStyleFloat(9.0f, 8.0f, ClimbStyle);

	FRotator RightHandRotation = FRotationMatrix::MakeFromX(RightHandClimbResult.ImpactNormal).Rotator();
	UParkourFunctionLibrary::ReverseRotation(RightHandRotation);
	RightHandRotation.Roll += 270.0f;
	RightHandRotation.Pitch += 270.0f;


	if (!AnimInstance->GetClass()->ImplementsInterface(UParkourABPInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("ClimbMoveRightHandIK: AnimInstance does not implement the ABP interface"));
		return;
	}

	IParkourABPInterface* ParkourABPInterface = Cast<IParkourABPInterface>(AnimInstance);
	ParkourABPInterface->Execute_SetRightHandLedgeLocation(AnimInstance, RightHandLocation);
	ParkourABPInterface->Execute_SetRightHandLedgeRotation(AnimInstance, RightHandRotation);
}

float UParkourComponent::GetClimbLeftHandZOffset() const
{
	if (ClimbDirection.GetTagName().IsEqual("Parkour.Direction.NoDirection"))
		return 0.0f;

	float BracedDirectionSubtract = ClimbDirection.GetTagName().IsEqual("Parkour.Direction.Right") ? 255.72f : 195.72f;
	float FreeHangDirectionSubtract = ClimbDirection.GetTagName().IsEqual("Parkour.Direction.Right") ? 229.09f : 227.6f;

	float OriginalOffset = AnimInstance->GetCurveValue("Hand_L Z") - UParkourFunctionLibrary::SelectClimbStyleFloat(BracedDirectionSubtract, FreeHangDirectionSubtract, ClimbStyle);

	return FMath::Clamp(OriginalOffset, 0.0f, 5.0f);
}

float UParkourComponent::GetClimbRightHandZOffset() const
{
	if (ClimbDirection.GetTagName().IsEqual("Parkour.Direction.NoDirection"))
		return 0.0f;

	float BracedDirectionSubtract = ClimbDirection.GetTagName().IsEqual("Parkour.Direction.Right") ? 254.37f : 194.36f;
	float FreeHangDirectionSubtract = ClimbDirection.GetTagName().IsEqual("Parkour.Direction.Right") ? 227.6f : 229.09f;

	float OriginalOffset = AnimInstance->GetCurveValue("Hand_R Z") - UParkourFunctionLibrary::SelectClimbStyleFloat(BracedDirectionSubtract, FreeHangDirectionSubtract, ClimbStyle);

	return FMath::Clamp(OriginalOffset, 0.0f, 5.0f);
}

void UParkourComponent::ClimbMoveFootIK()
{
	if (!ParkourStateTag.GetTagName().IsEqual("Parkour.State.Climb") || !bFirstClimbMove)
		return;

	ClimbMoveLeftFootIK();
	ClimbMoveRightFootIK();
}

void UParkourComponent::ClimbMoveLeftFootIK()
{
	if (AnimInstance->GetCurveValue("LeftFootIK") != 1.0f)
	{
		ResetFootIK(true);
		return;
	}

	FHitResult LeftFootHitResult;

	bool bShouldBreak = false;
	for (int32 i = 0; i < 5; i++)
	{
		for (int32 j = 0; j < 5; j++)
		{
			FVector StartLocation = CharacterMesh->GetSocketLocation("ik_foot_l");
			StartLocation.Z = CharacterMesh->GetSocketLocation("hand_l").Z - 135.0f + (j * 5.0f);
			StartLocation += Character->GetActorRightVector() * (13.0f + i * 4.0f);
			StartLocation -= Character->GetActorForwardVector() * 30.0f;

			FVector EndLocation = StartLocation + Character->GetActorForwardVector() * 100.0f;


			PerformSphereTraceByChannel(Character->GetWorld(), LeftFootHitResult, StartLocation, EndLocation, 15.0f, ECC_Visibility, bDrawDebug, 0.0f);

			if (!LeftFootHitResult.bBlockingHit || LeftFootHitResult.bStartPenetrating)
				continue;

			bShouldBreak = true;
			break;
		}

		if (bShouldBreak)
			break;
	}

	if (!bShouldBreak)
		return;

	FQuat QuatRotation = FRotationMatrix::MakeFromX(LeftFootHitResult.ImpactNormal).ToQuat();
	FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

	FVector LeftFootLocation = LeftFootHitResult.ImpactPoint + ForwardVector * 18.0f;

	if (!AnimInstance->GetClass()->ImplementsInterface(UParkourABPInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("ClimbMoveLeftFootIK: AnimInstance does not implement the ABP interface"));
		return;
	}

	IParkourABPInterface* ParkourABPInterface = Cast<IParkourABPInterface>(AnimInstance);
	ParkourABPInterface->Execute_SetLeftFootLocation(AnimInstance, LeftFootLocation);
}

void UParkourComponent::ClimbMoveRightFootIK()
{
	if (AnimInstance->GetCurveValue("RightFootIK") != 1.0f)
	{
		ResetFootIK(false);
		return;
	}

	FHitResult RightFootHitResult;

	bool bShouldBreak = false;
	for (int32 i = 0; i < 5; i++)
	{
		for (int32 j = 0; j < 5; j++)
		{
			FVector StartLocation = CharacterMesh->GetSocketLocation("ik_foot_r");
			StartLocation.Z = CharacterMesh->GetSocketLocation("hand_r").Z - 125.0f + (j * 5.0f);
			StartLocation += Character->GetActorRightVector() * (-13.0f + i * 4.0f);
			StartLocation -= Character->GetActorForwardVector() * 30.0f;

			FVector EndLocation = StartLocation + Character->GetActorForwardVector() * 100.0f;


			PerformSphereTraceByChannel(Character->GetWorld(), RightFootHitResult, StartLocation, EndLocation, 15.0f, ECC_Visibility, bDrawDebug, 0.0f);

			if (!RightFootHitResult.bBlockingHit || RightFootHitResult.bStartPenetrating)
				continue;

			bShouldBreak = true;
			break;
		}

		if (bShouldBreak)
			break;
	}

	if (!bShouldBreak)
		return;

	FQuat QuatRotation = FRotationMatrix::MakeFromX(RightFootHitResult.ImpactNormal).ToQuat();
	FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

	FVector RightFootLocation = RightFootHitResult.ImpactPoint + ForwardVector * 18.0f;

	if (!AnimInstance->GetClass()->ImplementsInterface(UParkourABPInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("ClimbMoveRightFootIK: AnimInstance does not implement the ABP interface"));
		return;
	}

	IParkourABPInterface* ParkourABPInterface = Cast<IParkourABPInterface>(AnimInstance);
	ParkourABPInterface->Execute_SetRightFootLocation(AnimInstance, RightFootLocation);
}

void UParkourComponent::ResetFootIK(bool bIsLeft)
{
	if (!AnimInstance->GetClass()->ImplementsInterface(UParkourABPInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("ResetFootIK: AnimInstance does not implement the ABP interface"));
		return;
	}

	IParkourABPInterface* ParkourABPInterface = Cast<IParkourABPInterface>(AnimInstance);

	if (bIsLeft)
		ParkourABPInterface->Execute_SetLeftFootLocation(AnimInstance, CharacterMesh->GetSocketLocation("ik_foot_l"));
	else
		ParkourABPInterface->Execute_SetRightFootLocation(AnimInstance, CharacterMesh->GetSocketLocation("ik_foot_r"));
}
