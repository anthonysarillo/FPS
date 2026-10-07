// Sarillo Creative Co.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "FPS/Types/ShooterTypes.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class UMaterialInstanceDynamic;
class UMaterialInterface;
enum EPhysicalSurface :int;

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	Auto,
	SemiAuto
};

UENUM(BlueprintType)
enum class EWeaponStatus : uint8
{
	Idle,
	Attacking,
	Reloading,
	Switching,
	Unequipped
};

UCLASS()
class FPS_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();
	
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	USkeletalMeshComponent* GetMesh3P() const { return Mesh3P; }
	
	void AttachToOwningPawn(APawn* Pawn) const;
	void DetachFromOwningPawn();
	
	void WeaponTrace(FHitResult& OutHit, float TraceLength);
	
	void Local_Attack(const FVector& ImpactPoint, const FVector& ImpactNormal, TEnumAsByte<EPhysicalSurface> ImpactSurfaceType, bool bIsFirstPerson);
	void Auth_Attack();
	void Rep_Attack(int32 AuthAmmo);
	
	UMaterialInstanceDynamic* GetReticleDMI();
	UMaterialInstanceDynamic* GetAmmoCounterDMI();
	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AimFOV;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TraceRadius = 5.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag WeaponType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EAttackType AttackType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FReticleParams ReticleParams;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AttackTime;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Capacity;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Ammo;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 StartingAmmo;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> WeaponIcon;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EWeaponStatus WeaponStatus = EWeaponStatus::Idle; 

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> Mesh1P;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> Mesh3P;
	
	void SetMeshVisibilities(APawn* OwningPawn) const;
	
	UFUNCTION(BlueprintImplementableEvent)
	void AttackEffects(const FVector& ImpactPoint, const FVector& ImpactNormal, EPhysicalSurface ImpactSurfaceType, bool bIsFirstPerson);
	
	int32 Sequence = 0;

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> ReticleMaterial; 
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> AmmoCounterMaterial;
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DMI_Reticle;
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DMI_AmmoCounter;
};
