// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetActor.h"

// Sets default values
AWidgetActor::AWidgetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
	RootComponent = SceneComponent;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget Component"));
	WidgetComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/ParkourSystemPlugin/Widgets/WBP_ParkourStats"));
	if (WidgetClassFinder.Succeeded())
	{
		WidgetComponent->SetWidgetClass(WidgetClassFinder.Class);
	}
	
	WidgetComponent->SetRelativeRotation(FRotator(0.0f, -150.0f, 0.0f));
	WidgetComponent->SetRelativeScale3D(FVector(0.07f,0.07f,0.07f));
	WidgetComponent->SetDrawSize(FVector2D(1000, 390));
}

// Called when the game starts or when spawned
void AWidgetActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWidgetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

