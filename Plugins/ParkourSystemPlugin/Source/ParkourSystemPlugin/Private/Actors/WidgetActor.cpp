// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetActor.h"
#include "UObject/ConstructorHelpers.h" 

// Sets default values
AWidgetActor::AWidgetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
	RootComponent = SceneComponent;

	//Create a widget component
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget Component"));
	WidgetComponent->SetupAttachment(RootComponent);

	//Find widget blueprint class
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/ParkourSystemPlugin/Widgets/WBP_ParkourStats"));
	if (WidgetClassFinder.Succeeded())
	{
		WidgetComponent->SetWidgetClass(WidgetClassFinder.Class);
	}
	
	//Set the location of the widget that will be visible from the character viewport
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

