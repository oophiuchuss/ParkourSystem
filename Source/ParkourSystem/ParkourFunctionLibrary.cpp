// Fill out your copyright notice in the Description page of Project Settings.


#include "ParkourFunctionLibrary.h"

FRotator UParkourFunctionLibrary::NormalReverseRotationZ(FVector Normal)
{
	FMatrix RotationMatrix = FRotationMatrix::MakeFromX(Normal);
	FRotator RotatorFromX = RotationMatrix.Rotator();

	RotatorFromX.Yaw += 180.f;
	RotatorFromX.Normalize();

	return RotatorFromX;
}

void UParkourFunctionLibrary::ReverseRotation(FRotator& Rotator)
{
	Rotator.Yaw += 180.f;
	Rotator.Normalize();
}

float UParkourFunctionLibrary::SelectClimbStyleFloat(float Braced, float FreeHang, const FGameplayTag& ClimbStyle)
{
	if (ClimbStyle.GetTagName().IsEqual("Parkour.ClimbStyle.Braced"))
	{
		return Braced;
	}
	else if (ClimbStyle.GetTagName().IsEqual("Parkour.ClimbStyle.FreeHang"))
	{
		return FreeHang;
	}

	return 0.0f;
}

float UParkourFunctionLibrary::SelectDirectionFloat(float Forward, float Backward, float Left, float Right,
	float ForwardLeft, float BackwardLeft, float ForwardRight, float BackwardRight, const FGameplayTag& Direction)
{
	if (Direction.GetTagName().IsEqual("Parkour.Direction.Forward"))
	{
		return Forward;
	}
	else if (Direction.GetTagName().IsEqual("Parkour.Direction.Backward"))
	{
		return Backward;
	}
	else if (Direction.GetTagName().IsEqual("Parkour.Direction.Left"))
	{
		return Left;
	}
	else if (Direction.GetTagName().IsEqual("Parkour.Direction.Right"))
	{
		return Right;
	}
	else if (Direction.GetTagName().IsEqual("Parkour.Direction.ForwardLeft"))
	{
		return ForwardLeft;
	}
	else if (Direction.GetTagName().IsEqual("Parkour.Direction.BackwardLeft"))
	{
		return BackwardLeft;
	}
	else if (Direction.GetTagName().IsEqual("Parkour.Direction.ForwardRight"))
	{
		return ForwardRight;
	}
	else if (Direction.GetTagName().IsEqual("Parkour.Direction.BackwardRight"))
	{
		return BackwardRight;
	}
	
	return 0.0f;
}

float UParkourFunctionLibrary::SelectParkourStateFloat(float NotBusy, float Vault, float Mantle, float Climb, const FGameplayTag& ParkourState)
{
	if (ParkourState.GetTagName().IsEqual("Parkour.State.NotBusy"))
	{
		return NotBusy;
	}
	else if (ParkourState.GetTagName().IsEqual("Parkour.State.Vault"))
	{
		return Vault;
	}
	else if (ParkourState.GetTagName().IsEqual("Parkour.State.Mantle"))
	{
		return Mantle;
	}
	else if (ParkourState.GetTagName().IsEqual("Parkour.State.Climb"))
	{
		return Climb;
	}
	return 0.0f;
}

const FGameplayTag& UParkourFunctionLibrary::SelectDirectionHopAction(const FGameplayTag& Forward, const FGameplayTag& Backward, const FGameplayTag& Left,
	const FGameplayTag& Right, const FGameplayTag& ForwardLeft, const FGameplayTag& BackwardLeft, const FGameplayTag& ForwardRight,
	const FGameplayTag& BackwardRight, const FGameplayTag& Direction)
{
	static FGameplayTag DefaultTag = FGameplayTag();
	if (Direction.GetTagName().IsEqual("Parkour.Direction.Forward"))
	{
		return Forward;
	}
	else if (Direction.GetTagName().IsEqual("Parkour.Direction.Backward"))
	{
		return Backward;
	}
	else if (Direction.GetTagName().IsEqual("Parkour.Direction.Left"))
	{
		return Left;
	}
	else if (Direction.GetTagName().IsEqual("Parkour.Direction.Right"))
	{
		return Right;
	}
	else if (Direction.GetTagName().IsEqual("Parkour.Direction.ForwardLeft"))
	{
		return ForwardLeft;
	}
	else if (Direction.GetTagName().IsEqual("Parkour.Direction.BackwardLeft"))
	{
		return BackwardLeft;
	}
	else if (Direction.GetTagName().IsEqual("Parkour.Direction.ForwardRight"))
	{
		return ForwardRight;
	}
	else if (Direction.GetTagName().IsEqual("Parkour.Direction.BackwardRight"))
	{
		return BackwardRight;
	}

	return DefaultTag;
}
