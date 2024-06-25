// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ArrowComponent.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArrowActor.generated.h"

/**
 * AArrowActor is a simple utility class that extends AActor.
 *
 * This actor is used to show the direction and orientation of a character.
 * Additionally used for calculations during side and corner movements during climbing
 */
UCLASS()
class PARKOURSYSTEMPLUGIN_API AArrowActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AArrowActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ArrowComponent used to visually represent the arrow.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* ArrowComponent;

	// SceneComponent for the base component hierarchy.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneComponent;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Getter for the ArrowComponent.
	UArrowComponent* GetArrowComponent();
};
