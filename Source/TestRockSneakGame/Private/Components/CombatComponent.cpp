// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CombatComponent.h"
#include "DrawDebugHelpers.h"
#include "SneakyPlayerController.h"
#include "Components/SlateWrapperTypes.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/InteractWithCrosshairs.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/Weapon.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCombatComponent::PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount)
{
	StartingPistolAmmo = FMath::Clamp(CarriedAmmo + AmmoAmount, 0, MaxCarriedAmmo);
	UpdateCarriedAmmo();
	
	if(EquippedWeapon && EquippedWeapon->IsEmpty() && EquippedWeapon->GetWeaponType() == WeaponType)
	{
		Reload();

	}
}
void UCombatComponent::UpdateAmmoValues()
{
	if(Character == nullptr || EquippedWeapon == nullptr) return;
	int32  ReloadAmount = AmountToReload();

	CarriedAmmo -= ReloadAmount;
	
	PC = PC == nullptr ? Cast<ASneakyPlayerController>(Character->Controller) : PC;
	if(PC)
	{
		PC->SetHUDCarriedAmmo(CarriedAmmo);
	}
	EquippedWeapon->AddAmmo(ReloadAmount);

	CombatState = ECombatState::Unoccupied;
	
}
int32 UCombatComponent::AmountToReload()
{
	if(EquippedWeapon == nullptr) return 0;
	int32 RoomInMag = EquippedWeapon->GetMagCapacity() - EquippedWeapon->GetAmmo();
	int32 AmountCarried = CarriedAmmo;
	int32 Least = FMath::Min(RoomInMag, AmountCarried);

	return FMath::Clamp(RoomInMag, 0, Least);
}
void UCombatComponent::Heal(float HealAmount, float HealingTime)
{
	bHealing = true;
	HealingRate = HealAmount / HealingTime;
	AmountToHeal += HealAmount;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FHitResult HitResult;
	// TraceUnderCrosshairs
	TraceUnderCrosshairs(HitResult);

	// Set Crosshairs
	SetHUDCrosshairs(DeltaTime);

	// HealMagnification
	HealMagnification(DeltaTime);

}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
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

void UCombatComponent::EquipPrimaryWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr) return;
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::Equipped);

	AttachActorToRightHand(EquippedWeapon);

	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->SetHUDAmmo();
	UpdateCarriedAmmo();
	ReloadEmptyWeapon();
}
void UCombatComponent::EquipSecondaryWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr) return;
	SecondaryWeapon = WeaponToEquip;
	SecondaryWeapon->SetWeaponState(EWeaponState::EquippedSecondary);
	AttachActorToRightPelvis(WeaponToEquip);

	SecondaryWeapon->SetOwner(Character);

}
void UCombatComponent::AttachActorToRightHand(AActor* ActorToAttach)
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

void UCombatComponent::AttachActorToRightPelvis(AActor* ActorToAttach)
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

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if(GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation (ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	// 2D screen space coordiante into a 3D
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);
	
	if(bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;

		if(Character)
		{
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 50.f);
			//DrawDebugSphere(GetWorld(), Start, 16.f, 12, FColor::Red, false);
		}
		
		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;
		
		GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECollisionChannel::ECC_Visibility);
		
		if(TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairs>())
		{
			HUDCrosshairStruct.Crosshairscolor = FLinearColor::Red;
		}
		else
		{
			HUDCrosshairStruct.Crosshairscolor = FLinearColor::White;
		}

		if(!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
			HitTarget = End;
		}
		// when success
		else
		{
			HitTarget = TraceHitResult.ImpactPoint;
			//DrawDebugSphere(GetWorld(), TraceHitResult.ImpactPoint, 12.f, 12.f, FColor::Red,
			//	false, -1);
		}
		//DrawDebugLine(GetWorld(), Start, FVector(Start + HitTarget * TRACE_LENGTH / HitTarget.Size()),
		//FColor::Purple, true);
	}
}
void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if(Character == nullptr || Character->Controller == nullptr) return;

	PC = PC == nullptr ? Cast<ASneakyPlayerController>(Character->Controller) : PC;
	if(PC)
	{
		HUD = HUD == nullptr ? Cast<ASneakyHUD>(PC->GetHUD()) : HUD;
		if(HUD)
		{
			if(EquippedWeapon)
			{
				if (EquippedWeapon->CrosshairsCenter == nullptr || EquippedWeapon->CrosshairsLeft == nullptr ||
					EquippedWeapon->CrosshairsRight == nullptr || EquippedWeapon->CrosshairsBottom == nullptr ||
					EquippedWeapon->CrosshairsTop == nullptr) return;
				
				HUDCrosshairStruct.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
				HUDCrosshairStruct.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				HUDCrosshairStruct.CrosshairsRight = EquippedWeapon->CrosshairsRight;
				HUDCrosshairStruct.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
				HUDCrosshairStruct.CrosshairsTop = EquippedWeapon->CrosshairsTop;
				HUD->SetHUDCrosshairStruct(HUDCrosshairStruct);
			}
			else
			{
				HUDCrosshairStruct.CrosshairsCenter = nullptr;
				HUDCrosshairStruct.CrosshairsLeft = nullptr;
				HUDCrosshairStruct.CrosshairsRight = nullptr;
				HUDCrosshairStruct.CrosshairsBottom = nullptr;
				HUDCrosshairStruct.CrosshairsTop = nullptr;
				HUD->SetHUDCrosshairStruct(HUDCrosshairStruct);
			}
			// Calculate crosshair spread
			//[0, 600] -> [0, 1]
			const FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			const FVector2D VelocityMultiplierRange(0.f, 1.f);
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f;

			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

			if(Character->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
			}
		}
	}

	
}

void UCombatComponent::UpdateCarriedAmmo()
{
	if(EquippedWeapon == nullptr) return;
	CarriedAmmo = StartingPistolAmmo;
	
	PC = PC == nullptr ? Cast<ASneakyPlayerController>(Character->Controller) : PC;
	if(PC)
	{
		PC->SetHUDCarriedAmmo(CarriedAmmo);
	}
}
void UCombatComponent::ReloadEmptyWeapon()
{
	if(EquippedWeapon && EquippedWeapon->IsEmpty())
	{
		Reload();
	}
}

void UCombatComponent::HealMagnification(float DeltaTime)
{
	if (!bHealing || Character == nullptr) return;

	const float HealThisFrame = HealingRate * DeltaTime;
	Character->Health = FMath::Clamp(Character->Health + HealThisFrame, 0.f, Character->MaxHealth);

	AmountToHeal -= HealThisFrame;

	if(AmountToHeal <= 0.f || Character->Health >= Character->MaxHealth)
	{
		bHealing = false;
		AmountToHeal = 0.f;
	}
}

bool UCombatComponent::CanFire()
{
	if (EquippedWeapon == nullptr) return false;
	if (!EquippedWeapon->IsEmpty() && CombatState == ECombatState::Reloading) return false;
	if (CombatState == ECombatState::Melee) return false;
	return !EquippedWeapon->IsEmpty() && CombatState == ECombatState::Unoccupied;
}

void UCombatComponent::Fire()
{
	if(CanFire())
	{
		if (EquippedWeapon && Character)
		{
			//==============================================================================
			// Use HitTarget From TraceUnderCrosshairs or Update it with TraceEndWithScatter
			//==============================================================================
			HitTarget = EquippedWeapon->bUseScatter ? EquippedWeapon->TraceEndWithScatter(HitTarget) : HitTarget;
			if(EquippedWeapon == nullptr) return;

			if(Character && CombatState == ECombatState::Unoccupied)
			{
				Character->PlayFireMontage();
				EquippedWeapon->Fire(HitTarget, false);
			}
		}
	}
	// Melee Weapon
	else if (EquippedWeapon->GetWeaponType() == EWeaponType::ExpandableBaton)
	{
		
	}
	else
	{
		ReloadEmptyWeapon();
	}
}
void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;

	if(bFireButtonPressed)
	{
		Fire();
	}
}
void UCombatComponent::SetAiming(bool bIsAiming)
{
	if(Character == nullptr || EquippedWeapon == nullptr) return;
	bAiming = bIsAiming;
}
void UCombatComponent::SwapWeapons()
{
	if (CombatState != ECombatState::Unoccupied) return;
	AWeapon* TempWeapon = EquippedWeapon;
	EquippedWeapon = SecondaryWeapon;
	SecondaryWeapon = TempWeapon;
	
	EquippedWeapon->SetWeaponState(EWeaponState::Equipped);
	AttachActorToRightHand(EquippedWeapon);
	EquippedWeapon->SetHUDAmmo();

	// Show / Hide Ammo Horizontal Box in MainPlayerOverlay
	PC = PC == nullptr ? Cast<ASneakyPlayerController>(Character->Controller) : PC;
	if(PC)
	{
		if(EquippedWeapon->GetWeaponType() == EWeaponType::Pistol)
		{

			PC->AmmoHorizontalBoxVisibility(ESlateVisibility::Visible);
		}
		else
		{
			PC->AmmoHorizontalBoxVisibility(ESlateVisibility::Hidden);
		}
	}
	
	SecondaryWeapon->SetWeaponState(EWeaponState::EquippedSecondary);
	AttachActorToRightPelvis(SecondaryWeapon);
	
}

void UCombatComponent::Reload()
{
	if(CarriedAmmo > 0 && CombatState == ECombatState::Unoccupied && EquippedWeapon && !EquippedWeapon->IsFull())
	{
		CombatState = ECombatState::Reloading;
		if (Character)
		{
			Character->PlayReloadMontage();
			if (EquippedWeapon->GetWeaponType() == EWeaponType::Pistol)
			{
				UpdateAmmoValues();
			}
		}
	}
}



