// Fill out your copyright notice in the Description page of Project Settings.


#include "ParkourComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ParkourFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "ParkourABPInterface.h"
#include "ParkourStatsInterface.h"


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
				//CheckMantleSurface();
				//CheckVaultSurface();

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
	bAutoClimb = false;
	bCanAutoClimb = true;
	bCanManualClimb = true;
	bShowHitResult = true;
	bDrawDebug = true;
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

	int Index = CharacterMovement->IsFalling() ? 8 : 15;

	bool bShouldBreak = false;
	for (int i = 0; i <= Index; ++i)
	{
		for (int j = 0; j <= 11; ++j)
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

	for (size_t i = 0; i <= Index; i++)
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

		for (size_t k = 0; k <= InnerIndex; k++)
		{
			StartLocation = LineHitResult.TraceStart + FVector(0.0f, 0.0f, k * 8.0f);
			EndLocation = LineHitResult.TraceEnd + FVector(0.0f, 0.0f, k * 8.0f);

			PerformLineTraceByChannel(Character->GetWorld(), CloneLineHitResult, StartLocation, EndLocation, ECC_Visibility);

			HopHitTraces.Add(CloneLineHitResult);
		}
	}

	for (int32 i = 1; i < HopHitTraces.Num(); ++i)
	{
		float Distance1 = HopHitTraces[i].bBlockingHit ? HopHitTraces[i].Distance : FVector::Distance(HopHitTraces[i].TraceStart, HopHitTraces[i].TraceEnd);
		float Distance2 = HopHitTraces[i - 1].bBlockingHit ? HopHitTraces[i - 1].Distance : FVector::Distance(HopHitTraces[i - 1].TraceStart, HopHitTraces[i - 1].TraceEnd);

		if (Distance1 - Distance2 > 5.0f)
		{
			WallHitTraces.Add(HopHitTraces[i - 1]);
			break;
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

void UParkourComponent::PerformSphereTraceByChannel(UWorld* World, FHitResult& HitResult, const FVector& StartLocation, const FVector& EndLocation, float Radius, ECollisionChannel TraceChannel)
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

void UParkourComponent::PerformLineTraceByChannel(UWorld* World, FHitResult& HitResult, const FVector& StartLocation, const FVector& EndLocation, ECollisionChannel CollisionChannel)
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

		UE_LOG(LogTemp, Warning, TEXT("%f, %f, %f"), WallHeight, WallDepth, VaultHeight);
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

	if (WallHeight >= 250)
	{
		SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
		return;
	}

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
	ParkourABPInterface->Execute_SetParkourAction(AnimInstance, NewParkourAction);

	if (!WidgetActor->WidgetComponent->GetWidget()->GetClass()->ImplementsInterface(UParkourStatsInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetParkourAction: Widget does not implement the Stats interface"));
		return;
	}

	IParkourStatsInterface* ParkourStatsInterface = Cast<IParkourStatsInterface>(WidgetActor->WidgetComponent->GetWidget());
	ParkourStatsInterface->Execute_SetParkourAction(WidgetActor->WidgetComponent->GetWidget(), NewParkourAction.GetTagName().ToString());
}

bool UParkourComponent::CheckMantleSurface()
{
	FVector StartLocation = WallTopResult.ImpactPoint + FVector(0.0f, 0.0f, CapsuleComponent->GetUnscaledCapsuleHalfHeight() + 8.0f);
	FVector EndLocation = StartLocation;
	float HalfHeight = CapsuleComponent->GetUnscaledCapsuleHalfHeight() - 8.0f;
	float Radius = 25.0f;

	FCollisionQueryParams TraceParams(FName(TEXT("CapsuleTrace")));

	FHitResult HitResult;
	Character->GetWorld()->SweepSingleByChannel(HitResult, StartLocation, EndLocation, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeCapsule(Radius, HalfHeight), TraceParams);
	if (bDrawDebug)
	{
		if (HitResult.bBlockingHit)
			DrawDebugCapsule(Character->GetWorld(), HitResult.ImpactPoint, Radius, HalfHeight, FQuat::Identity, FColor::Red, false, 2.0f);
		DrawDebugCapsule(Character->GetWorld(), StartLocation, Radius, HalfHeight, FQuat::Identity, FColor::Green, false, 2.0f);

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
			DrawDebugCapsule(Character->GetWorld(), HitResult.ImpactPoint, Radius, HalfHeight, FQuat::Identity, FColor::Red, false, 2.0f);
		DrawDebugCapsule(Character->GetWorld(), StartLocation, Radius, HalfHeight, FQuat::Identity, FColor::Green, false, 2.0f);

	}

	return !HitResult.bBlockingHit;
}

