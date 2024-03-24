// Fill out your copyright notice in the Description page of Project Settings.


#include "ParkourComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ParkourFunctionLibrary.h"

#include "DrawDebugHelpers.h"
#include "Engine/World.h" 
#include "CollisionQueryParams.h"

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
			}
		}
		else
		{
			if (bCanManualClimb)
			{
				ChekcWallShape();
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
			WidgetActor->SetActorRelativeLocation(FVector(60.0, 50.0, -3.0));
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

bool UParkourComponent::ChekcWallShape()
{
	int Index = CharacterMovement->IsFalling() ? 8 : 15;

	FHitResult HitResult;
	FHitResult HitResult2;
	FHitResult HitResult3;
	bool bShouldBreak = false;
	for (int i = 0; i <= Index; ++i)
	{
		for (int j = 0; j <= 11; ++j)
		{
			FVector StartLocation = Character->GetActorLocation() + FVector(0.0f, 0.0f, (i * 16.0f) - 60.0f) +
				(Character->GetActorForwardVector() * (-20.0f));
			FVector EndLocation = Character->GetActorLocation() + FVector(0.0f, 0.0f, (i * 16.0f) - 60.0f) +
				(Character->GetActorForwardVector() * (j * 10.0f + 10.0f));

			bool bHit = Character->GetWorld()->SweepSingleByChannel(HitResult, StartLocation, EndLocation, FQuat::Identity,
				ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(10.0f), FCollisionQueryParams(FName(TEXT("SphereTrace")), true));

			if (bHit)
			{
				DrawDebugSphere(Character->GetWorld(), HitResult.ImpactPoint, 10.0f,
					32, FColor::Red, false, 2.0f);
			}
			DrawDebugSphere(Character->GetWorld(), EndLocation, 10.0f,
				8,FColor::Green, false, 2.0f);

			if (bHit && !HitResult.bStartPenetrating)
			{
				bShouldBreak = true;
				break;
			}
		}

		if (bShouldBreak)
			break;
	}


	if (!HitResult.bBlockingHit || HitResult.bStartPenetrating)
		return false;

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

		FVector StartLocation = (RightVector * (i * 20.0f + UParkourFunctionLibrary::SelectParkourStateFloat(-40.0f, 0.0f, 0.0f, -20.0f, ParkourStateTag))) + ImpactPoint + (ForwardVector * -40.0f);
		FVector EndLocation = (RightVector * (i * 20.0f + UParkourFunctionLibrary::SelectParkourStateFloat(-40.0f, 0.0f, 0.0f, -20.0f, ParkourStateTag))) + ImpactPoint + (ForwardVector * 30.0f);

		Character->GetWorld()->LineTraceSingleByChannel(HitResult2, StartLocation, EndLocation, ECC_Visibility);

		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 1.0f);
		if (HitResult2.bBlockingHit)
		{
			DrawDebugPoint(GetWorld(), HitResult2.ImpactPoint, 10.0f, FColor::Green, false, 1.0f);
		}

		HopHitTraces.Empty();

		int InnerIndex = UParkourFunctionLibrary::SelectParkourStateFloat(30.0f, .0f, .0f, 7.0f, ParkourStateTag);

		for (size_t k = 0; k <= InnerIndex; k++)
		{
			StartLocation = HitResult2.TraceStart;
			StartLocation.Z += k * 8.0f;
			EndLocation = HitResult2.TraceEnd;
			EndLocation.Z += k * 8.0f;


			Character->GetWorld()->LineTraceSingleByChannel(HitResult3, StartLocation, EndLocation, ECC_Visibility);

			DrawDebugLine(Character->GetWorld(), StartLocation, EndLocation, FColor::Green, false, 1.0f);

			if (HitResult3.bBlockingHit)
			{
				DrawDebugPoint(GetWorld(), HitResult3.ImpactPoint, 10.0f, FColor::Green, false, 1.0f);
			}

			HopHitTraces.Add(HitResult3);
		}

	}


	for (int32 i = 0; i < HopHitTraces.Num(); ++i)
	{
		if (i != 0)
		{
			float Distance1 = HopHitTraces[i].bBlockingHit ? HopHitTraces[i].Distance : FVector::Distance(HopHitTraces[i].TraceStart, HopHitTraces[i].TraceEnd);
			float Distance2 = HopHitTraces[i - 1].bBlockingHit ? HopHitTraces[i - 1].Distance : FVector::Distance(HopHitTraces[i - 1].TraceStart, HopHitTraces[i - 1].TraceEnd);

			if (Distance1 - Distance2 > 5.0f)
			{
				WallHitTraces.Add(HopHitTraces[i - 1]);
				break;
			}
		}
	}

	for (int32 i = 0; i < WallHitTraces.Num(); ++i)
	{

		if (i == 0) WallHitResult = WallHitTraces[i];
		else
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

			FVector StartLocation = (ForwardVector * i * 30.0f) + (ForwardVector * 2.0f) + WallHitResult.ImpactPoint + FVector(.0f, .0f, 7.0f);

			FVector EndLocation = StartLocation - FVector(.0f, .0f, 7.0f);

			bool bHit = Character->GetWorld()->SweepSingleByChannel(HitResult, StartLocation, EndLocation, FQuat::Identity,
				ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(2.5f), FCollisionQueryParams(FName(TEXT("SphereTrace")), true));

			if (bHit)
			{
				DrawDebugSphere(Character->GetWorld(), HitResult.ImpactPoint, 2.5f,
					32, FColor::Red, false, 2.0f);
			}
			DrawDebugSphere(Character->GetWorld(), StartLocation, 2.5f,
				8, FColor::Green, false, 2.0f);


			if (i == 0)
			{
				if (bHit)
				{
					WallTopResult = HitResult;
				}
			}
			else
			{
				if (bHit)
				{
					TopHits = HitResult;
				}
				else
				{
					break;
				}
			}
		}

		if (ParkourStateTag.GetTagName().IsEqual("Parkour.State.NotBusy"))
		{
			FQuat QuatRotation = FQuat(WallRotation);

			FVector ForwardVector = QuatRotation.RotateVector(FVector::ForwardVector);
			

			bool bHit = Character->GetWorld()->SweepSingleByChannel(HitResult, TopHits.ImpactPoint + (ForwardVector * 30.0f), TopHits.ImpactPoint, FQuat::Identity,
				ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(2.5f), FCollisionQueryParams(FName(TEXT("SphereTrace")), true));

			if (bHit)
			{
				DrawDebugSphere(Character->GetWorld(), HitResult.ImpactPoint, 2.5f,
					32, FColor::Red, false, 2.0f);
			}
			DrawDebugSphere(Character->GetWorld(), TopHits.ImpactPoint + (ForwardVector * 30.0f), 2.5f,
				8, FColor::Green, false, 2.0f);

			if (bHit)
			{
				WallDepthResult = HitResult;

				FVector Start = WallDepthResult.ImpactPoint + ForwardVector * 70.0f;

				bHit = Character->GetWorld()->SweepSingleByChannel(HitResult, Start, Start - FVector(.0f, .0f, 200.0f), FQuat::Identity,
					ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(10.0f), FCollisionQueryParams(FName(TEXT("SphereTrace")), true));


				if (bHit)
				{
					DrawDebugSphere(Character->GetWorld(), HitResult.ImpactPoint, 10.0f,
						32, FColor::Red, false, 2.0f);
				}
				DrawDebugSphere(Character->GetWorld(), Start, 10.0f,
					8, FColor::Green, false, 2.0f);

				if (bHit)
				{
					WallVaultResult = HitResult;
				}
			}
		}
	}

	return false;
}



