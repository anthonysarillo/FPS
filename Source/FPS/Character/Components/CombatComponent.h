// Sarillo Creative Co.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "FPS/Types/ShooterTypes.h"
#include "CombatComponent.generated.h"

class UAnimMontage;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class AWeapon;
class UWeaponData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReticleChanged, UMaterialInstanceDynamic*, ReticleDMI, const FReticleParams&, ReticleParams, bool, bCurrentlyTargetingPlayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAmmoCounterChanged, UMaterialInstanceDynamic*, AmmoCounterDMI, int32, CurrentRounds, int32, MaxRounds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRoundFired, int32, RoundsCurrent, int32, RoundsMax, int32, RoundsInReserve);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAimingStatusChanged, bool, bIsAiming);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTargetingPlayerStatusChanged, bool, bIsAiming);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCurrentReserveAmmoChanged, int32, RoundsInReserve, int32, RoundsInWeapon, UMaterialInterface*, WeaponIconMaterial);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPS_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintPure)
	static UCombatComponent* FindCombatComponent(const AActor* Actor) { return (IsValid(Actor) ? Actor->FindComponentByClass<UCombatComponent>() : nullptr ); }
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UWeaponData> WeaponData;
	
	UPROPERTY(BlueprintReadOnly, Transient, ReplicatedUsing=OnRep_CurrentWeapon)
	TObjectPtr<AWeapon> CurrentWeapon;
	
	UPROPERTY(ReplicatedUsing= OnRep_CurrentReserveAmmo)
	int32 CurrentReserveAmmo;
	
	TMap<FGameplayTag, int32> ReserveAmmo;
	
	bool bHitPlayer;
	bool bHitPlayerLastFrame;
	
	void SwitchWeapon();
	void Local_SwitchWeapon(int32 WeaponIndex);
	UFUNCTION(Server, Reliable)
	void Server_SwitchWeapon(int32 WeaponIndex);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SwitchWeapon(int32 WeaponIndex);
	void Notify_SwitchWeapon();
	UFUNCTION()
	void BlendOut_SwitchWeapon(UAnimMontage* Montage, bool bInterrupted);
	
	void ToggleAim();
	void Local_ToggleAim(bool bIsAiming);
	UFUNCTION(Server, Reliable)
	void Server_ToggleAim(bool bIsAiming);
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bAiming;
	
	void StartAttack();
	void Local_Attack();
	UFUNCTION(Server, Reliable)
	void Server_Attack(const FHitResult& Hit);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Attack(const FHitResult& Hit, int32 AuthAmmo);
	
	void StopAttack();
	
	void Reload();
	void Local_Reload();
	UFUNCTION(Server, Reliable)
	void Server_Reload();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Reload(int32 NewWeaponAmmo, int32 NewCarriedAmmo);
	
	void SpawnInventory();
	void DestroyInventory();
	
	void Equip(AWeapon* Weapon);
	void EquipWeapon(AWeapon* Weapon);
	UFUNCTION(Server, Reliable)
	void Server_EquipWeapon(AWeapon* Weapon);
	
	void InitWeaponWidgets() const;
	
	UPROPERTY(BlueprintAssignable)
	FReticleChanged OnReticleChanged;
	
	UPROPERTY(BlueprintAssignable)
	FAmmoCounterChanged OnAmmoCounterChanged;
	
	UPROPERTY(BlueprintAssignable)
	FRoundFired OnRoundFired;
	
	UPROPERTY(BlueprintAssignable)
	FAimingStatusChanged OnAimingStatusChanged;
	
	UPROPERTY(BlueprintAssignable)
	FTargetingPlayerStatusChanged OnTargetingPlayerStatusChanged;
	
	UPROPERTY(BlueprintAssignable)
	FCurrentReserveAmmoChanged OnCurrentReserveAmmoChanged;

protected:
	UPROPERTY(Transient, Replicated)
	TArray<AWeapon*> Inventory;
	
	UFUNCTION()
	void OnRep_CurrentWeapon(AWeapon* LastWeapon);
	
	UFUNCTION()
	void OnRep_CurrentReserveAmmo();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<AWeapon>> DefaultWeaponClasses;
	
	AWeapon* SpawnWeapon(TSubclassOf<AWeapon> WeaponClass) const;
	
	UPROPERTY(EditDefaultsOnly)
	float TraceLength = 20000.f;
	
	bool bTriggerPressed = false;
	FTimerHandle AttackTimer;
	void AttackTimerFinished();
	
private:
	int32 Local_WeaponIndex = 0;
	
	int32 AdvanceWeaponIndex();
	void SetCurrentWeapon(AWeapon* NewWeapon, AWeapon* LastWeapon);
	
	
	
};
