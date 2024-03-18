#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ParkourStatsInterface.generated.h"

UINTERFACE(MinimalAPI)
class UParkourStatsInterface : public UInterface
{
    GENERATED_BODY()
};

class IParkourStatsInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ParkourSystem")
    bool SetParkourState(const FString& ParkourState);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ParkourSystem")
    bool SetParkourAction(const FString& ParkourAction);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ParkourSystem")
    bool SetClimbStyle(const FString& ClimbStyle);
};
