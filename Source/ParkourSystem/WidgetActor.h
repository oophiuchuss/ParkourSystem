// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Components/WidgetComponent.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WidgetActor.generated.h"

UCLASS()
class PARKOURSYSTEM_API AWidgetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWidgetActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USceneComponent* SceneComponent;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* WidgetComponent;
};
