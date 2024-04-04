// Fill out your copyright notice in the Description page of Project Settings.


#include "ParkourComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ParkourFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "ParkourABPInterface.h"
#include "ParkourStatsInterface.h"
#include "ThinVaultDT.h"
#include "VaultDT.h"
#include "HighVaultDT.h"
#include "MantleDT.h"
#include "LowMantleDT.h"
#include "BracedClimbDT.h"
#include "FreeHangDT.h"

// Sets default values for this component's properties
UParkourComponent::UParkourComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

//TODO: there sould be bool
void UParkourComponent::ParkourAction()
{
	if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.NoAction"))
	{

		if (bAutoClimb)
		{
			if (bCanAutoClimb)
			{
				ChekcWallShape();
				ShowHitResults();
				CheckDistance();
				ParkourType(false);
			}
		}
		else
		{
			if (bCanManualClimb)
			{
				ChekcWallShape();
				ShowHitResults();
				CheckDistance();
				ParkourType(false);
			}
		}
	}

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

	// ...
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
	ParkourActionTag = FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction");
	ParkourStateTag = FGameplayTag::RequestGameplayTag("Parkour.State.NotBusy");
	ClimbStyle = FGameplayTag::RequestGameplayTag("Parkour.ClimbStyle.FreeHang");
	bAutoClimb = false;
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
			WidgetActor->SetActorRelativeLocation(FVector(100.0, 50.0, -3.0));
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

			ArrowLocationX = 0;
			ArrowLocationZ = 100;
			CharacterHeight = 0;
			ArrowActor->SetActorRelativeLocation(FVector(ArrowLocationX, 0.0f, ArrowLocationZ - CharacterHeight));
		}
		else
			return false;



		if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(ParkourMappingContext, 0);

				if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
				{
					EnhancedInputComponent->BindAction(ParkourInputAction, ETriggerEvent::Started, this, &UParkourComponent::ParkourAction);
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
		FirstTargetArmLenght = CameraBoom->TargetArmLength;
		FirstTargetRelativeLocation = CameraBoom->GetComponentLocation();
	}
	else
		return false;

	return true;
}

void UParkourComponent::ChekcWallShape()
{
	FHitResult HitResult;
	FHitResult LineHitResult;
	FHitResult CloneLineHitResult;

	FVector StartLocation;
	FVector EndLocation;

	int32 Index = CharacterMovement->IsFalling() ? 8 : 15;

	bool bShouldBreak = false;
	for (int32 i = 0; i <= Index; ++i)
	{
		for (int32 j = 0; j <= 11; ++j)
		{
			StartLocation = Character->GetActorLocation() + FVector(0.0f, 0.0f, (i * 16.0f) - 60.0f) +
				(Character->GetActorForwardVector() * (-20.0f));
			EndLocation = Character->GetActorLocation() + FVector(0.0f, 0.0f, (i * 16.0f) - 60.0f) +
				(Character->GetActorForwardVector() * (j * 10.0f + 10.0f));

			PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 10.0f, ECC_Visibility);

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

		PerformLineTraceByChannel(Character->GetWorld(), LineHitResult, StartLocation, EndLocation, ECC_Visibility);

		HopHitTraces.Empty();

		int InnerIndex = UParkourFunctionLibrary::SelectParkourStateFloat(30.0f, .0f, .0f, 7.0f, ParkourStateTag);

		for (int32 k = 0; k <= InnerIndex; k++)
		{
			StartLocation = LineHitResult.TraceStart + FVector(0.0f, 0.0f, k * 8.0f);
			EndLocation = LineHitResult.TraceEnd + FVector(0.0f, 0.0f, k * 8.0f);

			PerformLineTraceByChannel(Character->GetWorld(), CloneLineHitResult, StartLocation, EndLocation, ECC_Visibility);

			HopHitTraces.Add(CloneLineHitResult);
		}

		for (int32 j = 1; j < HopHitTraces.Num(); ++j)
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

			for (int32 i = 1; i < WallHitTraces.Num(); ++i)
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

			for (int32 i = 0; i <= 8; ++i)
			{
				FQuat QuatRotation = FQuat(WallRotation);
				FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

				StartLocation = (ForwardVector * (i * 30.0f + 2.0f)) + WallHitResult.ImpactPoint + FVector(0.0f, 0.0f, 7.0f);
				EndLocation = StartLocation - FVector(0.0f, 0.0f, 7.0f);

				PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 2.5f, ECC_Visibility);

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

				PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 2.5f, ECC_Visibility);

				if (HitResult.bBlockingHit)
				{
					WallDepthResult = HitResult;

					StartLocation = WallDepthResult.ImpactPoint + ForwardVector * 70.0f;
					EndLocation = StartLocation - FVector(0.0f, 0.0f, 200.0f);

					PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 10.0f, ECC_Visibility);


					if (HitResult.bBlockingHit)
					{
						WallVaultResult = HitResult;
					}
				}
			}
		}
	}
}

void UParkourComponent::PerformSphereTraceByChannel(UWorld* World, FHitResult& HitResult, const FVector& StartLocation, const FVector& EndLocation, float Radius, ECollisionChannel TraceChannel) const
{
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid world reference."));
		return;
	}

	FCollisionQueryParams TraceParams(FName(TEXT("SphereTrace")));

	World->SweepSingleByChannel(HitResult, StartLocation, EndLocation, FQuat::Identity, TraceChannel, FCollisionShape::MakeSphere(Radius), TraceParams);

	if (bDrawDebug) {
		if (HitResult.bBlockingHit)
			DrawDebugSphere(World, HitResult.ImpactPoint, Radius, 32, FColor::Red, false, 2.0f);
		DrawDebugSphere(World, StartLocation, Radius, 8, FColor::Green, false, 2.0f);
	}
}

void UParkourComponent::PerformLineTraceByChannel(UWorld* World, FHitResult& HitResult, const FVector& StartLocation, const FVector& EndLocation, ECollisionChannel CollisionChannel) const
{
	World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, CollisionChannel);

	if (bDrawDebug)
	{
		DrawDebugLine(World, StartLocation, EndLocation, FColor::Green, false, 1.0f);
		if (HitResult.bBlockingHit)
		{
			DrawDebugPoint(World, HitResult.ImpactPoint, 10.0f, FColor::Green, false, 1.0f);
		}
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

void UParkourComponent::ParkourType(bool AutoClimb)
{
	if (!WallTopResult.bBlockingHit)
	{
		SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
		if (!AutoClimb)
		{
			Character->Jump();
		}
		return;
	}

	if (!ParkourStateTag.GetTagName().IsEqual("Parkour.State.NotBusy"))
	{
		if (ParkourStateTag.GetTagName().IsEqual("Parkour.State.Climb"))
		{
			return;
		}
		return;
	}

	if (!bOnGround)
	{
		return;
	}

	if (WallHeight > 0 && WallHeight <= 44)
	{
		SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
		return;
	}

	if (WallHeight > 44 && WallHeight < 90)
	{
		if (CheckMantleSurface())
		{
			SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.LowMantle"));
		}
		else
		{
			SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
		}
		return;
	}

	if (WallHeight > 90 && WallHeight <= 160 && VaultHeight <= 160 && WallDepth >= 0 && WallDepth <= 120)
	{
		if (WallHeight <= 120 && VaultHeight <= 120 && WallDepth <= 30)
		{
			if (CheckVaultSurface())
				SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.ThinVault"));
			else
				SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));

			return;
		}

		if (WallDepth >= 0 && WallDepth <= 120 && CharacterMovement->Velocity.Length() > 20)
		{
			if (WallHeight <= 120 && VaultHeight <= 120)
			{
				if (CheckVaultSurface())
					SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.Vault"));
				else
					SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));

				return;
			}

			if (WallHeight > 120 && WallHeight <= 160 && VaultHeight <= 160)
			{
				if (CheckVaultSurface())
					SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.HighVault"));
				else
					SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));

				return;
			}
		}
	}

	if (WallHeight > 90 && WallHeight <= 160)
		if (CheckMantleSurface())
			SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.Mantle"));
		else
			SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));

	if (WallHeight < 250)
	{
		if (CheckClimbSurface())
		{
			CheckClimbStyle();
			SecondClimbLedgeResultCalculation();

			if(ClimbStyle.GetTagName().IsEqual("Parkour.ClimbStyle.Braced"))
				SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.Climb"));
			else
				SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.FreeHangClimb"));
		}
	}


	/*if (WallHeight > 90 && WallHeight <= 120)
	{
		if (WallDepth >= 0 && WallDepth <= 120)
		{
			if (VaultHeight >= 60 && VaultHeight <= 120)
			{
				if (WallDepth >= 0 && WallDepth <= 30)
				{
					if (CheckVaultSurface())
					{
						SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.ThinVault"));
						return;
					}
					else
					{
						SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
						return;
					}

				}
				else
					if (CharacterMovement->Velocity.Length() > 20)
					{
						if (CheckVaultSurface())
						{
							SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.Vault"));
							return;
						}
						else
						{
							SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
							return;
						}
					}
					else
						if (CheckMantleSurface())
						{
							SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.Mantle"));
							return;
						}
						else
						{
							SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
							return;
						}

			}
			else
				if (CheckMantleSurface())
				{
					SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.Mantle"));
					return;
				}
				else
				{
					SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
					return;
				}
		}
		else
			if (CheckMantleSurface())
			{
				SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.Mantle"));
				return;
			}
			else
			{
				SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
				return;
			}
	}
	else
	{
		if (WallHeight > 44 && WallHeight <= 90)
		{
			if (CheckMantleSurface())
			{
				SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.LowMantle"));
			}
			else
			{
				SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
			}
		}
		else
		{
			if (WallHeight > 120 && WallHeight <= 160)
			{
				if (WallDepth > 0 && WallDepth <= 120)
				{
					if (VaultHeight >= 60 && VaultHeight <= 120)
					{
						if (CharacterMovement->Velocity.Length() > 20)
						{
							if (CheckVaultSurface())
							{
								SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.Vault"));
								return;
							}
							else
							{
								SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
								return;
							}
						}
						else
						{
							if (CheckMantleSurface())
							{
								SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.Mantle"));
								return;
							}
							else
							{
								SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
								return;
							}
						}
					}
					else
					{
						if (CheckMantleSurface())
						{
							SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.Mantle"));
							return;
						}
						else
						{
							SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
							return;
						}
					}
				}
				else
				{
					if (CheckMantleSurface())
					{
						SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.Mantle"));
						return;
					}
					else
					{
						SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
						return;
					}

				}
			}
		}
	}*/
	/*if (WallHeight > 44 && WallHeight <= 90)
	{
		if (CheckMantleSurface())
		{
			SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.LowMantle"));
		}
		else
		{
			SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
		}
		return;
	}

	if (WallHeight > 90 && WallHeight <= 120 && WallDepth >= 0 && WallDepth <= 120)
	{
		if (VaultHeight >= 60 && VaultHeight <= 120 && WallDepth <= 30)
		{
			if (CheckVaultSurface())
			{
				SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.ThinVault"));
			}
			else
			{
				SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
			}
		}
		else if (CharacterMovement->Velocity.Length() > 20)
		{
			if (CheckVaultSurface())
			{
				SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.Vault"));
			}
			else
			{
				SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
			}
		}
		else
		{
			if (CheckMantleSurface())
			{
				SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.Mantle"));
			}
			else
			{
				SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
			}
		}
		return;
	}

	if (WallHeight > 120 && WallHeight <= 160 && WallDepth > 0 && WallDepth <= 120)
	{
		if (VaultHeight >= 60 && VaultHeight <= 120 && CharacterMovement->Velocity.Length() > 20)
		{
			if (CheckVaultSurface())
			{
				SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.HighVault"));
			}
			else
			{
				SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
			}
		}
		else
		{
			if (CheckMantleSurface())
			{
				SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.Mantle"));
			}
			else
			{
				SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
			}
		}
		return;
	}*/
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
	else if (ParkourActionTag.GetTagName().IsEqual("Parkour.Action.NoAction"))
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

	ParkourStateTag = NewParkourState;

	if (!AnimInstance->GetClass()->ImplementsInterface(UParkourABPInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetParkourAction: AnimInstance does not implement the ABP interface"));
		return;
	}

	IParkourABPInterface* ParkourABPInterface = Cast<IParkourABPInterface>(AnimInstance);
	ParkourABPInterface->Execute_SetParkourState(AnimInstance, ParkourStateTag);

	if (!WidgetActor->WidgetComponent->GetWidget()->GetClass()->ImplementsInterface(UParkourStatsInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetParkourAction: Widget does not implement the Stats interface"));
		return;
	}

	IParkourStatsInterface* ParkourStatsInterface = Cast<IParkourStatsInterface>(WidgetActor->WidgetComponent->GetWidget());
	ParkourStatsInterface->Execute_SetParkourState(WidgetActor->WidgetComponent->GetWidget(), ParkourStateTag.GetTagName().ToString());

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
		UE_LOG(LogTemp, Warning, TEXT("SetParkourAction: AnimInstance does not implement the ABP interface"));
		return;
	}

	IParkourABPInterface* ParkourABPInterface = Cast<IParkourABPInterface>(AnimInstance);
	ParkourABPInterface->Execute_SetClimbStyle(AnimInstance, ClimbStyle);

	if (!WidgetActor->WidgetComponent->GetWidget()->GetClass()->ImplementsInterface(UParkourStatsInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetParkourAction: Widget does not implement the Stats interface"));
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

bool UParkourComponent::CheckMantleSurface()
{
	FVector StartLocation = WallTopResult.ImpactPoint + FVector(0.0f, 0.0f, CapsuleComponent->GetUnscaledCapsuleHalfHeight() + 8.0f);
	FVector EndLocation = StartLocation;
	float HalfHeight = CapsuleComponent->GetUnscaledCapsuleHalfHeight() - 8.0f;
	float Radius = 25.0f;

	FCollisionQueryParams TraceParams(FName(TEXT("CapsuleTrace")));

	FHitResult HitResult;
	Character->GetWorld()->SweepSingleByChannel(HitResult, StartLocation, EndLocation, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeBox(FVector(20.0f)), TraceParams);
	if (bDrawDebug)
	{
		if (HitResult.bBlockingHit || HitResult.bStartPenetrating)
			DrawDebugCapsule(Character->GetWorld(), HitResult.ImpactPoint, HalfHeight, Radius, FQuat::Identity, FColor::Red, false, 2.0f);
		DrawDebugCapsule(Character->GetWorld(), StartLocation, HalfHeight, Radius, FQuat::Identity, FColor::Green, false, 2.0f);
	}

	return !HitResult.bBlockingHit;
}

bool UParkourComponent::CheckVaultSurface()
{
	FVector StartLocation = WallTopResult.ImpactPoint + FVector(0.0f, 0.0f, (CapsuleComponent->GetUnscaledCapsuleHalfHeight() / 2) + 18.0f);
	FVector EndLocation = StartLocation;
	float HalfHeight = (CapsuleComponent->GetUnscaledCapsuleHalfHeight() / 2) + 5.0f;
	float Radius = 25.0f;

	FCollisionQueryParams TraceParams(FName(TEXT("CapsuleTrace")));

	FHitResult HitResult;
	Character->GetWorld()->SweepSingleByChannel(HitResult, StartLocation, EndLocation, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeCapsule(Radius, HalfHeight), TraceParams);
	if (bDrawDebug)
	{
		if (HitResult.bBlockingHit)
			DrawDebugCapsule(Character->GetWorld(), HitResult.ImpactPoint, HalfHeight, Radius, FQuat::Identity, FColor::Red, false, 2.0f);
		DrawDebugCapsule(Character->GetWorld(), StartLocation, HalfHeight, Radius, FQuat::Identity, FColor::Green, false, 2.0f);

	}

	return !HitResult.bBlockingHit;
}

bool UParkourComponent::CheckClimbSurface()
{
	FQuat QuatRotation = FQuat(WallRotation);
	FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

	FVector StartLocation = WallTopResult.ImpactPoint + FVector(0.0f, 0.0f, -90.0f) + (ForwardVector * -55.0f);
	float HalfHeight = 82.0f;
	float Radius = 25.0f;

	FCollisionQueryParams TraceParams(FName(TEXT("CapsuleTrace")));

	FHitResult HitResult;
	Character->GetWorld()->SweepSingleByChannel(HitResult, StartLocation, StartLocation, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeBox(FVector(20.0f)), TraceParams);
	if (bDrawDebug)
	{
		if (HitResult.bBlockingHit || HitResult.bStartPenetrating)
			DrawDebugCapsule(Character->GetWorld(), HitResult.ImpactPoint, HalfHeight, Radius, FQuat::Identity, FColor::Red, false, 2.0f);
		DrawDebugCapsule(Character->GetWorld(), StartLocation, HalfHeight, Radius, FQuat::Identity, FColor::Green, false, 2.0f);
	}

	return !HitResult.bBlockingHit;
}

void UParkourComponent::CheckClimbStyle()
{
	FQuat QuatRotation = FQuat(WallRotation);
	FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

	FVector StartLocation = WallTopResult.ImpactPoint + FVector(0.0f, 0.0f, -125.0f) + (ForwardVector * -10.0f);
	FVector EndLocation = StartLocation + (ForwardVector * 40.0f);
	FHitResult HitResult;
	PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 10.0f, ECC_Visibility);

	if (HitResult.bBlockingHit)
		SetClimbStyle(FGameplayTag::RequestGameplayTag("Parkour.ClimbStyle.Braced"));
	else
		SetClimbStyle(FGameplayTag::RequestGameplayTag("Parkour.ClimbStyle.FreeHang"));
}

void UParkourComponent::SecondClimbLedgeResultCalculation()
{
	FQuat QuatRotation = FQuat(UParkourFunctionLibrary::NormalReverseRotationZ(WallHitResult.ImpactNormal));
	FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

	FVector StartLocation = WallHitResult.ImpactPoint + (ForwardVector * 30.0f);
	FVector EndLocation = WallHitResult.ImpactPoint - (ForwardVector * 30.0f);

	PerformSphereTraceByChannel(Character->GetWorld(), SecondClimbLedgeResult, StartLocation, EndLocation, 5.0f, ECC_Visibility);

	if (!SecondClimbLedgeResult.bBlockingHit)
		return;

	WallRotation = UParkourFunctionLibrary::NormalReverseRotationZ(SecondClimbLedgeResult.ImpactNormal);
	QuatRotation = FQuat(WallRotation);
	ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);

	StartLocation = SecondClimbLedgeResult.ImpactPoint + (ForwardVector * 2.0f) + FVector(0.0f, 0.0f, 5.0f);
	EndLocation = StartLocation - FVector(0.0f, 0.0f, 55.0f);
	
	FHitResult HitResult;
	PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, EndLocation, 5.0f, ECC_Visibility);

	SecondClimbLedgeResult.Location = HitResult.Location;
	SecondClimbLedgeResult.ImpactPoint.Z = HitResult.ImpactPoint.Z;
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


	UAnimMontage* AnimMontage = ParkourVariables->ParkourMontage;
	if (!AnimMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayParkourMontage: AnimMontage wasn't found"));
		return;
	}
	float StartTimeInSeconds = ParkourVariables->MontageStartPosition;

	AnimInstance->Montage_Play(AnimMontage, 1.0f, EMontagePlayReturnType::MontageLength, StartTimeInSeconds);
}

void UParkourComponent::OnParkourMontageBlendOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
		return;

	//TODO There should be ParkourVariables pointer
	SetParkourState(BlendOutState);
	SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
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

	FHitResult HitResult;
	PerformSphereTraceByChannel(Character->GetWorld(), HitResult, StartLocation, StartLocation - FVector(0.0f, 0.0f, 60.0f), 25.0f, ECC_Visibility);

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
	SecondClimbLedgeResult = FHitResult();
}

