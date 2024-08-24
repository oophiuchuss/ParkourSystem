// Fill out your copyright notice in the Description page of Project Settings.


#include "ArrowActor.h"

// Sets default values
AArrowActor::AArrowActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
	RootComponent = SceneComponent;

	SceneComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));

	//Create an arrow component itself
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(RootComponent);
	ArrowComponent->SetHiddenInGame(true);
}

// Called when the game starts or when spawned
void AArrowActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AArrowActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UArrowComponent* AArrowActor::GetArrowComponent()
{
	return ArrowComponent;
}

