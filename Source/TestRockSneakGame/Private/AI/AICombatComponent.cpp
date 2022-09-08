// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AICombatComponent.h"
#include "Components/CombatComponent.h"
#include "DrawDebugHelpers.h"
#include "AI/Enemy.h"
#include "AI/EnemyController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/Weapon.h"


UAICombatComponent::UAICombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAICombatComponent::BeginPlay()
{
	Super::BeginPlay();
	TimeBetweenShots = 60 / RateOfFire;
}
void UAICombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAICombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if(Character == nullptr || WeaponToEquip == nullptr) return;
	if(CombatState != ECombatState::Unoccupied) return;
	
	if (EquippedWeapon != nullptr && SecondaryWeapon == nullptr)
	{
		EquipSecondaryWeapon(WeaponToEquip);
	}
	else
	{
		EquipPrimaryWeapon(WeaponToEquip);
	}
}

void UAICombatComponent::EquipPrimaryWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr) return;
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::Equipped);

	AttachActorToRightHand(EquippedWeapon);

	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->SetHUDAmmo();
	ReloadEmptyWeapon();
}
void UAICombatComponent::EquipSecondaryWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr) return;
	SecondaryWeapon = WeaponToEquip;
	SecondaryWeapon->SetWeaponState(EWeaponState::EquippedSecondary);
	AttachActorToRightPelvis(WeaponToEquip);

	SecondaryWeapon->SetOwner(Character);

}
void UAICombatComponent::AttachActorToRightHand(AActor* ActorToAttach)
{
	if(Character == nullptr || Character->GetMesh() == nullptr || ActorToAttach == nullptr) return;
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if(HandSocket)
	{
		HandSocket->AttachActor(ActorToAttach, Character->GetMesh());
		if (EquippedWeapon->GetWeaponType() == EWeaponType::ExpandableBaton)
		{
			EquippedWeapon->SetActorRelativeLocation(FVector(2.f,8.f,22.f));
			EquippedWeapon->SetActorRelativeRotation(FRotator(-90.f,0,0));
		}
	}
}

void UAICombatComponent::AttachActorToRightPelvis(AActor* ActorToAttach)
{
	if (Character == nullptr || Character->GetMesh() == nullptr || ActorToAttach == nullptr) return;
	const USkeletalMeshSocket* RightPelvisSocket = Character->GetMesh()->GetSocketByName(FName("RightPelvisSocket"));
	if (RightPelvisSocket)
	{
		RightPelvisSocket->AttachActor(ActorToAttach, Character->GetMesh());
		if (SecondaryWeapon->GetWeaponType() == EWeaponType::Pistol)
		{
			SecondaryWeapon->SetActorRelativeLocation(FVector(10.f,-10.f ,2.f));
			SecondaryWeapon->SetActorRelativeRotation(FRotator(-90.f,0,100.f));
		}
	}
}

void UAICombatComponent::ReloadEmptyWeapon()
{
	if(EquippedWeapon && EquippedWeapon->IsEmpty())
	{
		Reload();
	}
}

void UAICombatComponent::Fire()
{
	if (EquippedWeapon && Character)
	{
		//==============================================================================
		// Use HitTarget From TraceUnderCrosshairs or Update it with TraceEndWithScatter
		//==============================================================================
		APawn* OwnerPawn = Cast<APawn>(GetOwner());
		if(OwnerPawn == nullptr) return;
		FVector EyeLocation;
		FRotator EyeRotation;
		OwnerPawn->GetActorEyesViewPoint(EyeLocation, EyeRotation);
		const FVector ShotDirection = EyeRotation.Vector();
		
		FVector TraceEnd = EyeLocation + (ShotDirection * TRACE_LENGTH);
		
		HitTarget = EquippedWeapon->bUseScatter ? EquippedWeapon->TraceEndWithScatter(TraceEnd) : HitTarget;
		if(EquippedWeapon == nullptr) return;

		if(Character && CombatState == ECombatState::Unoccupied)
		{
			//Character->PlayFireMontage();
			EquippedWeapon->Fire(HitTarget, true);
		}
		LastFireTime = GetWorld()->TimeSeconds;
	}
}
void UAICombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;

	if(bFireButtonPressed)
	{
		Fire();
	}
}
void UAICombatComponent::AIStartFire()
{
	const float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &UAICombatComponent::Fire, TimeBetweenShots, true, FirstDelay);
}


void UAICombatComponent::AIStopFire()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}
void UAICombatComponent::SetAiming(bool bIsAiming)
{
	if(Character == nullptr || EquippedWeapon == nullptr) return;
	bAiming = bIsAiming;
}
void UAICombatComponent::SwapWeapons()
{
	if (CombatState != ECombatState::Unoccupied) return;
	AWeapon* TempWeapon = EquippedWeapon;
	EquippedWeapon = SecondaryWeapon;
	SecondaryWeapon = TempWeapon;
	
	EquippedWeapon->SetWeaponState(EWeaponState::Equipped);
	AttachActorToRightHand(EquippedWeapon);
	EquippedWeapon->SetHUDAmmo();

	SecondaryWeapon->SetWeaponState(EWeaponState::EquippedSecondary);
	AttachActorToRightPelvis(SecondaryWeapon);
	
}

void UAICombatComponent::Reload()
{
	if(CarriedAmmo > 0 && CombatState == ECombatState::Unoccupied && EquippedWeapon && !EquippedWeapon->IsFull())
	{
		CombatState = ECombatState::Reloading;
		if (Character)
		{
			//Character->PlayReloadMontage();	
		}
	}
}
