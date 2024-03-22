// Fill out your copyright notice in the Description page of Project Settings.


#include "ParkourComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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

			}
		}
		else
		{
			if (bCanManualClimb)
			{

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
	bAutoClimb = false;
	bCanAutoClimb = false;
	bCanManualClimb = false;

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
			ArrowActor->SetActorRelativeLocation(FVector(ArrowLocationX, 0.0f, ArrowLocationZ-CharacterHeight));
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
					EnhancedInputComponent->BindAction(ParkourInputAction, ETriggerEvent::Triggered, this, &UParkourComponent::ParkourAction);
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
	int index = 0;
	if (CharacterMovement->IsFalling())
		index = 8;
	else
		index = 15;

	for (size_t i = 0; i < index; i++)
	{
		for (size_t j = 0; j < 11; j++)
		{
			FHitResult HitResult;
			//bool bHit = Character->GetWorld()->SweepSingleByChannel(HitResult, )
		}
	}

	return false;
}


