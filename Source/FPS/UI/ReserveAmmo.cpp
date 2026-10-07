// Sarillo Creative Co.


#include "ReserveAmmo.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "FPS/Character/ShooterCharacter.h"
#include "FPS/Weapon/Weapon.h"

void UReserveAmmo::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Image_WeaponIcon->SetRenderOpacity(0.f);
	Text_Ammo->SetRenderOpacity(0.f);
	
	GetOwningPlayer()->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);
	
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwningPlayer()->GetPawn());
	if (!IsValid(ShooterCharacter)) return;
	OnPossessedPawnChanged(nullptr, ShooterCharacter);
	
	if (ShooterCharacter->HasWeaponFirstReplicated())
	{
		AWeapon* Weapon = ICharacterInterface::Execute_GetCurrentWeapon(ShooterCharacter);
		if (IsValid(Weapon))
		{
			// Reserve Ammo amount for the current weapon, CurrentWeapon->Ammo
			OnCurrentReserveAmmoChanged(ICharacterInterface::Execute_GetReserveAmmo(ShooterCharacter), Weapon->Ammo, Weapon->WeaponIcon);
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
		OnCurrentReserveAmmoChanged(ICharacterInterface::Execute_GetReserveAmmo(ShooterCharacter), Weapon->Ammo, Weapon->WeaponIcon);
	}
}

void UReserveAmmo::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	UCombatComponent* OldPawnCombatComponent = UCombatComponent::FindCombatComponent(OldPawn);
	if (IsValid(OldPawnCombatComponent))
	{
		OldPawnCombatComponent->OnCurrentReserveAmmoChanged.RemoveDynamic(this, &ThisClass::OnCurrentReserveAmmoChanged);
		OldPawnCombatComponent->OnRoundFired.RemoveDynamic(this, &ThisClass::OnRoundFired);
	}
	UCombatComponent* NewPawnCombatComponent = UCombatComponent::FindCombatComponent(NewPawn);
	if (IsValid(NewPawnCombatComponent))
	{
		Image_WeaponIcon->SetRenderOpacity(1.f);
		Text_Ammo->SetRenderOpacity(1.f);
		
		NewPawnCombatComponent->OnCurrentReserveAmmoChanged.AddDynamic(this, &ThisClass::OnCurrentReserveAmmoChanged);
		NewPawnCombatComponent->OnRoundFired.AddDynamic(this, &ThisClass::OnRoundFired);
	}
}

void UReserveAmmo::OnCurrentReserveAmmoChanged(int32 RoundsInReserve, int32 RoundsInWeapon, UMaterialInterface* WeaponIconMaterial)
{
	if (IsValid(WeaponIconMaterial))
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(WeaponIconMaterial);
		if (IsValid(Image_WeaponIcon))
		{
			Image_WeaponIcon->SetBrush(Brush);
		}
	}

	if (IsValid(Text_Ammo))
	{
		FText AmmoText = FText::Format(NSLOCTEXT("AmmoText", "AmmoKey", "{0}/{1}" ), RoundsInWeapon, RoundsInReserve);
		Text_Ammo->SetText(AmmoText);
	}
}

void UReserveAmmo::OnRoundFired(int32 RoundsCurrent, int32 RoundsMax, int32 RoundsInReserve)
{
	if (IsValid(Text_Ammo))
	{
		FText AmmoText = FText::Format(NSLOCTEXT("AmmoText", "AmmoKey", "{0}/{1}" ), RoundsCurrent, RoundsInReserve);
		Text_Ammo->SetText(AmmoText);
	}
}

void UReserveAmmo::OnWeaponFirstReplicated(AWeapon* Weapon, bool bTargetingPlayer)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwningPlayer()->GetPawn());
	if (!IsValid(ShooterCharacter)) return;
	
	OnCurrentReserveAmmoChanged(ICharacterInterface::Execute_GetReserveAmmo(ShooterCharacter), Weapon->Ammo, Weapon->WeaponIcon);
}
