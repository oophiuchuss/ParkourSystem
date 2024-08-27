#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ParkourStatsInterface.generated.h"

UINTERFACE(Blueprintable, MinimalAPI)
class UParkourStatsInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * Interface for managing parkour-related stats and states for widget blueprints.
 */
class IParkourStatsInterface
{
    GENERATED_BODY()

public:
    // Sets the current parkour state.
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ParkourSystem")
    bool SetParkourState(const FString& ParkourState);

    // Sets the current parkour action.
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ParkourSystem")
    bool SetParkourAction(const FString& ParkourAction);

    // Sets the current climb style.
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ParkourSystem")
    bool SetClimbStyle(const FString& ClimbStyle);

    // Sets the current climb direction.
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ParkourSystem")
    bool SetClimbDirection(const FString& ClimbDirection);
};
