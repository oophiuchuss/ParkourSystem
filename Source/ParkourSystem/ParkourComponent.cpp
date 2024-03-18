// Fill out your copyright notice in the Description page of Project Settings.


#include "ParkourComponent.h"


// Sets default values for this component's properties
UParkourComponent::UParkourComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
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
			WidgetActor->SetActorRelativeLocation(FVector(30.0, 8.0, -3.0));
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


