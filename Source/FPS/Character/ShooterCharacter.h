// Sarillo Creative Co.

#pragma once

#include "CoreMinimal.h"
#include "Components/CombatComponent.h"
#include "FPS/Interfaces/CharacterInterface.h"
#include "FPS/Types/ShooterTypes.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

class AWeapon;
class UCombatComponent;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponFirstReplicated, AWeapon*, Weapon, bool, bTargetingPlayer);

UCLASS()
class FPS_API AShooterCharacter : public ACharacter, public ICharacterInterface
{
	GENERATED_BODY()

public:
	AShooterCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void Tick(float DeltaTime) override;
	
	// ICharacterInterface
	virtual FName GetWeaponSocket_Implementation(const FGameplayTag& WeaponType) const override;
	virtual USkeletalMeshComponent* GetMesh1P_Implementation() const override { return Mesh1P; }
	virtual USkeletalMeshComponent* GetMesh3P_Implementation() const override { return GetMesh(); }
	virtual void WeaponReplicated_Implementation() override;
	virtual AWeapon* GetCurrentWeapon_Implementation() override { return CombatComponent->CurrentWeapon; }
	virtual int32 GetReserveAmmo_Implementation() const override { return CombatComponent->CurrentReserveAmmo; }
	virtual void Notify_SwitchWeapon_Implementation() override;
	// end of ICharacterInterface
	
	UFUNCTION(BlueprintCallable)
	FRotator GetFixedAimRotation() const;
	
	UFUNCTION(BlueprintCallable)
	bool HasCurrentWeapon() const;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTransform FABRIK_SocketTransform;
	
	UPROPERTY(BlueprintAssignable)
	FWeaponFirstReplicated OnWeaponFirstReplicated;
	
	bool HasWeaponFirstReplicated() const { return bWeaponFirstReplicated; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> Mesh1P;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> FirstPersonCamera;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DefaultFOV = 90.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCombatComponent> CombatComponent;
	
	UPROPERTY(EditAnywhere, Category = "FPS|Input")
	TObjectPtr<UInputMappingContext> ShooterContext;
	
	UPROPERTY(EditAnywhere, Category = "FPS|Input")
	TObjectPtr<UInputAction> Input_Move;
	
	void Move(const FInputActionValue& InputActionValue);

	UPROPERTY(EditAnywhere, Category = "FPS|Input")
	TObjectPtr<UInputAction> Input_Look;
	
	void Look(const FInputActionValue& InputActionValue);
	
	UPROPERTY(EditAnywhere, Category = "FPS|Input")
	TObjectPtr<UInputAction> Input_ToggleCrouch;
	
	void ToggleCrouch();
	
	UPROPERTY(EditAnywhere, Category = "FPS|Input")
	TObjectPtr<UInputAction> Input_SwitchWeapon;
	
	void SwitchWeapon();
	
	UPROPERTY(EditAnywhere, Category = "FPS|Input")
	TObjectPtr<UInputAction> Input_ToggleAim;
	
	void ToggleAim();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnToggleAim(bool bIsAiming);
	
	UPROPERTY(EditAnywhere, Category = "FPS|Input")
	TObjectPtr<UInputAction> Input_Attack;
	
	void StartAttack();
	void StopAttack();
	
	UPROPERTY(EditAnywhere, Category = "FPS|Input")
	TObjectPtr<UInputAction> Input_Reload;
	
	void Reload();
	
	void CalculateFABRIKSocketTransform();
	void CalculateTurnInPlaceInfo(float DeltaTime);
	void TurnInPlace(float DeltaTime);
	
	UPROPERTY(BlueprintReadOnly)
	float AO_Yaw;
	
	UPROPERTY(BlueprintReadOnly)
	float MovementOffsetYaw;
	
	UPROPERTY(BlueprintReadOnly)
	ETurningInPlace TurningStatus = ETurningInPlace::NotTurning;
	
private:
	FRotator StartingAimRotation;
	float InterpAO_Yaw;
	
	bool bWeaponFirstReplicated = false;
	
};


