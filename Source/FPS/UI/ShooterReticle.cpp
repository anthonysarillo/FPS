// Sarillo Creative Co.


#include "ShooterReticle.h"

#include "Components/Image.h"
#include "FPS/Character/ShooterCharacter.h"
#include "FPS/Character/Components/CombatComponent.h"
#include "FPS/Weapon/Weapon.h"

namespace Ammo
{
	const FName Rounds_Current = FName("Rounds_Current");
	const FName Rounds_Max = FName("Rounds_Max");
}

namespace Reticle
{
	const FName RoundedCornerScale = FName("RoundedCornerScale");
	const FName ShapeCutThickness = FName("ShapeCutThickness");
	const FName Inner_RGBA = FName("Inner_RGBA");
}

void UShooterReticle::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Image_Reticle->SetRenderOpacity(0.f);
	Image_AmmoCounter->SetRenderOpacity(0.f);
	_BaseCornerScaleFactor_RoundFired = 0.f;
	_BaseShapeCutFactor_RoundFired = 0.f;
	_BaseCornerScaleFactor_Aiming = 0.f;
	_BaseShapeCutFactor_Aiming = 0.f;
	bAiming = false;
	bTargetingPlayer = false;
	
	GetOwningPlayer()->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);
	
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwningPlayer()->GetPawn());
	if (!IsValid(ShooterCharacter)) return;
	UCombatComponent* CombatComponent = UCombatComponent::FindCombatComponent(ShooterCharacter);
	if (!IsValid(CombatComponent)) return;
	
	OnPossessedPawnChanged(nullptr, ShooterCharacter);

	if (ShooterCharacter->HasWeaponFirstReplicated())
	{
		AWeapon* Weapon = ICharacterInterface::Execute_GetCurrentWeapon(ShooterCharacter);
		if (IsValid(Weapon))
		{
			OnReticleChanged(Weapon->GetReticleDMI(), Weapon->ReticleParams, CombatComponent->bHitPlayer);
			OnAmmoCounterChanged(Weapon->GetAmmoCounterDMI(), Weapon->Ammo, Weapon->Capacity);
		}
	}
	else
	{
		ShooterCharacter->OnWeaponFirstReplicated.AddDynamic(this, &ThisClass::OnWeaponFirstReplicated);
	}
	if (ShooterCharacter->HasAuthority())
	{
		AWeapon* Weapon = ICharacterInterface::Execute_GetCurrentWeapon(ShooterCharacter);
		if (!IsValid(Weapon)) return;
		OnReticleChanged(Weapon->GetReticleDMI(), Weapon->ReticleParams, CombatComponent->bHitPlayer);
		OnAmmoCounterChanged(Weapon->GetAmmoCounterDMI(), Weapon->Ammo, Weapon->Capacity);
	}
}

void UShooterReticle::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	_BaseCornerScaleFactor_RoundFired = FMath::FInterpTo(_BaseCornerScaleFactor_RoundFired, 0.f, InDeltaTime, CurrentReticleParams.RoundFiredInterpSpeed);
	_BaseShapeCutFactor_RoundFired = FMath::FInterpTo(_BaseShapeCutFactor_RoundFired, 0.f, InDeltaTime, CurrentReticleParams.RoundFiredInterpSpeed);
	
	_BaseCornerScaleFactor_Aiming = FMath::FInterpTo(_BaseCornerScaleFactor_Aiming, bAiming ? CurrentReticleParams.ScaleFactor_Aiming : CurrentReticleParams.ScaleFactor_NotAiming, InDeltaTime, CurrentReticleParams.AimingInterpSpeed);
	_BaseShapeCutFactor_Aiming = FMath::FInterpTo(_BaseShapeCutFactor_Aiming, bAiming ? CurrentReticleParams.ScaleFactor_Aiming : CurrentReticleParams.ScaleFactor_NotAiming, InDeltaTime, CurrentReticleParams.AimingInterpSpeed);
	
	_BaseCornerScaleFactor_TargetingPlayer = FMath::FInterpTo(_BaseCornerScaleFactor_TargetingPlayer, bTargetingPlayer ? CurrentReticleParams.ScaleFactor_TargetingPlayer : CurrentReticleParams.ScaleFactor_NotTargetingPlayer, InDeltaTime, CurrentReticleParams.TargetingPlayerInterpSpeed);
	
	BaseCornerScaleFactor = _BaseCornerScaleFactor_RoundFired + _BaseCornerScaleFactor_Aiming;
	BaseShapeCutFactor = _BaseShapeCutFactor_RoundFired + _BaseShapeCutFactor_Aiming;
	
	if (CurrentReticle_DMI.IsValid())
	{
		CurrentReticle_DMI->SetScalarParameterValue(Reticle::RoundedCornerScale, BaseCornerScaleFactor);
		CurrentReticle_DMI->SetScalarParameterValue(Reticle::ShapeCutThickness, BaseShapeCutFactor);
	}
}

void UShooterReticle::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	UCombatComponent* OldPawnCombatComponent = UCombatComponent::FindCombatComponent(OldPawn);
	if (IsValid(OldPawnCombatComponent))
	{
		OldPawnCombatComponent->OnReticleChanged.RemoveDynamic(this, &ThisClass::OnReticleChanged);
		OldPawnCombatComponent->OnAmmoCounterChanged.RemoveDynamic(this, &ThisClass::OnAmmoCounterChanged);
		OldPawnCombatComponent->OnRoundFired.RemoveDynamic(this, &ThisClass::OnRoundFired);
		OldPawnCombatComponent->OnAimingStatusChanged.RemoveDynamic(this, &ThisClass::OnAimingStatusChanged);
		OldPawnCombatComponent->OnTargetingPlayerStatusChanged.RemoveDynamic(this, &ThisClass::OnTargetingPlayerStatusChanged);
	}
	UCombatComponent* NewPawnCombatComponent = UCombatComponent::FindCombatComponent(NewPawn);
	if (IsValid(NewPawnCombatComponent))
	{
		Image_Reticle->SetRenderOpacity(1.f);
		Image_AmmoCounter->SetRenderOpacity(1.f);
		NewPawnCombatComponent->OnReticleChanged.AddDynamic(this, &ThisClass::OnReticleChanged);
		NewPawnCombatComponent->OnAmmoCounterChanged.AddDynamic(this, &ThisClass::OnAmmoCounterChanged);
		NewPawnCombatComponent->OnRoundFired.AddDynamic(this, &ThisClass::OnRoundFired);
		NewPawnCombatComponent->OnAimingStatusChanged.AddDynamic(this, &ThisClass::OnAimingStatusChanged);
		NewPawnCombatComponent->OnTargetingPlayerStatusChanged.AddDynamic(this, &ThisClass::OnTargetingPlayerStatusChanged);
	}
}

void UShooterReticle::OnWeaponFirstReplicated(AWeapon* Weapon, bool bIsTargetingPlayer)
{
	OnReticleChanged(Weapon->GetReticleDMI(), Weapon->ReticleParams, bIsTargetingPlayer);
	OnAmmoCounterChanged(Weapon->GetAmmoCounterDMI(), Weapon->Ammo, Weapon->Capacity);
}

void UShooterReticle::OnReticleChanged(UMaterialInstanceDynamic* ReticleDMI, const FReticleParams& ReticleParams, bool bCurrentlyTargetingPlayer)
{
	CurrentReticleParams = ReticleParams; 
	CurrentReticle_DMI = ReticleDMI;
	
	FSlateBrush Brush;
	Brush.SetResourceObject(ReticleDMI);
	if (IsValid(Image_Reticle))
	{
		Image_Reticle->SetBrush(Brush);
	}
	
	OnTargetingPlayerStatusChanged(bCurrentlyTargetingPlayer);
}

void UShooterReticle::OnAmmoCounterChanged(UMaterialInstanceDynamic* AmmoCounterDMI, int32 RoundsCurrent, int32 RoundsMax)
{
	CurrentAmmoCounter_DMI = AmmoCounterDMI;
	CurrentAmmoCounter_DMI->SetScalarParameterValue(Ammo::Rounds_Current, RoundsCurrent);
	CurrentAmmoCounter_DMI->SetScalarParameterValue(Ammo::Rounds_Max, RoundsMax);
	
	FSlateBrush Brush;
	Brush.SetResourceObject(AmmoCounterDMI);
	if (IsValid(Image_AmmoCounter))
	{
		Image_AmmoCounter->SetBrush(Brush);
	}
}

void UShooterReticle::OnRoundFired(int32 RoundsCurrent, int32 RoundsMax, int32 RoundsInReserve)
{
	_BaseCornerScaleFactor_RoundFired += CurrentReticleParams.ScaleFactor_RoundFired;
	_BaseShapeCutFactor_RoundFired += CurrentReticleParams.ShapeCutFactor_RoundFired;
	
	if (CurrentAmmoCounter_DMI.IsValid())
	{
		CurrentAmmoCounter_DMI->SetScalarParameterValue(Ammo::Rounds_Current, RoundsCurrent);
		CurrentAmmoCounter_DMI->SetScalarParameterValue(Ammo::Rounds_Max, RoundsMax);
	}
}

void UShooterReticle::OnAimingStatusChanged(bool bIsAiming)
{
	bAiming =  bIsAiming;
}

void UShooterReticle::OnTargetingPlayerStatusChanged(bool bTargeting)
{
	bTargetingPlayer = bTargeting;
	if (CurrentReticle_DMI.IsValid())
	{
		FLinearColor ReticleColor = bTargetingPlayer ? FLinearColor::Red : FLinearColor::White;
		CurrentReticle_DMI->SetVectorParameterValue(Reticle::Inner_RGBA, ReticleColor);
	}
}
