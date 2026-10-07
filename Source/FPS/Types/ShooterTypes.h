// Sarillo Creative Co.

#pragma once

#include "ShooterTypes.generated.h"

UENUM(BlueprintType)
enum class ETurningInPlace : uint8
{
	TurningLeft,
	TurningRight,
	NotTurning
};

USTRUCT(BlueprintType)
struct FReticleParams
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShapeCutFactor_RoundFired = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShapeCutFactor_Aiming = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShapeCutFactor_NotAiming = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ScaleFactor_RoundFired = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ScaleFactor_Aiming = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ScaleFactor_NotAiming = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ScaleFactor_TargetingPlayer= 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ScaleFactor_NotTargetingPlayer= 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RoundFiredInterpSpeed = 20.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AimingInterpSpeed = 15.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TargetingPlayerInterpSpeed = 10.f;
};


