// Sarillo Creative Co.


#include "Weapon.h"

#include "KismetTraceUtils.h"
#include "FPS/FPS.h"
#include "FPS/Interfaces/CharacterInterface.h"
#include "Kismet/KismetMathLibrary.h"


AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
	
	SetRootComponent(Mesh1P);
	
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh1P");
	Mesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetHiddenInGame(true);
	
	
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh3P");
	Mesh3P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh3P->bReceivesDecals = false;
	Mesh3P->CastShadow = true;
	Mesh3P->SetHiddenInGame(true);
	Mesh3P->SetupAttachment(Mesh1P);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::SetMeshVisibilities(APawn* OwningPawn) const
{
	if (OwningPawn->IsLocallyControlled())
	{
		Mesh1P->SetHiddenInGame(false);
		Mesh3P->SetHiddenInGame(true);
	}
	else
	{
		Mesh1P->SetHiddenInGame(true);
		Mesh3P->SetHiddenInGame(false);
	}
}

void AWeapon::AttachToOwningPawn(APawn* Pawn) const
{
	if (!IsValid(Pawn) || !Pawn->Implements<UCharacterInterface>()) return;
	
	SetMeshVisibilities(Pawn);
	
	const FName WeaponSocket = ICharacterInterface::Execute_GetWeaponSocket(Pawn, WeaponType);
	USkeletalMeshComponent* PawnMesh1P = ICharacterInterface::Execute_GetMesh1P(Pawn);
	USkeletalMeshComponent* PawnMesh3P = ICharacterInterface::Execute_GetMesh3P(Pawn);
	
	Mesh1P->AttachToComponent(PawnMesh1P, FAttachmentTransformRules::KeepRelativeTransform, WeaponSocket);
	Mesh3P->AttachToComponent(PawnMesh3P, FAttachmentTransformRules::KeepRelativeTransform, WeaponSocket);
}

void AWeapon::DetachFromOwningPawn()
{
	Mesh1P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	Mesh1P->SetHiddenInGame(true);
	
	Mesh3P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	Mesh3P->SetHiddenInGame(true);
	
}

void AWeapon::WeaponTrace(FHitResult& OutHit, float TraceLength)
{
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;
	QueryParams.AddIgnoredActor(GetOwner());
	
	FCollisionResponseParams ResponseParams;
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block);
	ResponseParams.CollisionResponse.SetResponse(ECC_WorldStatic, ECR_Block);
	ResponseParams.CollisionResponse.SetResponse(ECC_WorldDynamic, ECR_Block);
	ResponseParams.CollisionResponse.SetResponse(ECC_PhysicsBody, ECR_Block);
	
	ensure(GetInstigator());
	if (APlayerController* PC = Cast<APlayerController>(GetInstigator()->GetController()); IsValid(PC))
	{
		FVector EyesWorldLocation;
		FRotator EyesWorldRotation;
		PC->GetActorEyesViewPoint(EyesWorldLocation, EyesWorldRotation);
		const FVector EyesWorldDirection = UKismetMathLibrary::GetForwardVector(EyesWorldRotation);
		
		const FVector Start = EyesWorldLocation;
		const FVector End = Start + EyesWorldDirection * TraceLength;
		
		const bool bHit = GetWorld()->SweepSingleByChannel(
			OutHit,
			Start, End,
			FQuat::Identity,
			FPSTraceChannels::ECC_Weapon,
			FCollisionShape::MakeSphere(TraceRadius),
			QueryParams,
			ResponseParams);
		
		if (!bHit)
		{
			OutHit.ImpactPoint = End;
		}
		
		/*
		DrawDebugSphereTraceSingle(
			GetWorld(),
			Start,
			End,
			TraceRadius,
			EDrawDebugTrace::ForDuration,
			bHit,
			OutHit,
			FColor::Green,
			FColor::Red,
			3.f);
			*/
	}
}

void AWeapon::Local_Attack(const FVector& ImpactPoint, const FVector& ImpactNormal,
	TEnumAsByte<EPhysicalSurface> ImpactSurfaceType, bool bIsFirstPerson)
{
	AttackEffects(ImpactPoint, ImpactNormal, ImpactSurfaceType, bIsFirstPerson);
	
	if (GetInstigator()->IsLocallyControlled())
	{
		Ammo = FMath::Clamp(Ammo - 1, 0, Capacity);
		++Sequence;
	}
}

void AWeapon::Auth_Attack()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, Capacity);
}

void AWeapon::Rep_Attack(int32 AuthAmmo)
{
	if (GetInstigator()->IsLocallyControlled())
	{
		Ammo = AuthAmmo;
		--Sequence;
		Ammo-=Sequence;
	}
}

UMaterialInstanceDynamic* AWeapon::GetReticleDMI()
{
	if (!IsValid(DMI_Reticle))
	{
		DMI_Reticle = UMaterialInstanceDynamic::Create(ReticleMaterial, this);
	}
	return DMI_Reticle;
}

UMaterialInstanceDynamic* AWeapon::GetAmmoCounterDMI()
{
	if (!IsValid(DMI_AmmoCounter))
	{
		DMI_AmmoCounter = UMaterialInstanceDynamic::Create(AmmoCounterMaterial, this);
	}
	return DMI_AmmoCounter;
}




