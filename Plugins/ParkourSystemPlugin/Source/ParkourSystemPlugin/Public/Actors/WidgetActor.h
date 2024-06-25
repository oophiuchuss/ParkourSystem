// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Components/WidgetComponent.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WidgetActor.generated.h"

/**
 * AWidgetActor is an actor that includes a widget component.
 * 
 * It is used for having a representetive view of all states, actions and climb styles in real time 
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API AWidgetActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWidgetActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Scene component of the actor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneComponent;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Widget component attached to the actor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* WidgetComponent;
};
