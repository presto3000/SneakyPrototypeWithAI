// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Definitions.h"
#include "PlayerCharacter.h"
#include "UI/SneakyHUD.h"
#include "CombatComponent.generated.h"


class ASneakyHUD;
class ASneakyPlayerController;
class AWeapon;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TESTROCKSNEAKGAME_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	void EquipWeapon(AWeapon* WeaponToEquip);
	void FireButtonPressed(bool bPressed);
	void SetAiming(bool bIsAiming);
	void SwapWeapons();
	void Reload();
	UPROPERTY()
	ECombatState CombatState = ECombatState::Unoccupied;
	void PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount);
	void Heal(float HealAmount, float HealingTime);
protected:
	virtual void BeginPlay() override;
	bool bAiming = false;
	bool bFireButtonPressed = false;
	bool CanFire();
	bool bCanFire = true;
	void Fire();

	
	void EquipPrimaryWeapon(AWeapon* WeaponToEquip);
	void EquipSecondaryWeapon(AWeapon* WeaponToEquip);


	void AttachActorToRightHand(AActor* ActorToAttach);
	//void AttachActorToLeftHand(AActor* ActorToAttach);
	void AttachActorToRightPelvis(AActor* ActorToAttach);
	
	void UpdateCarriedAmmo();
	void ReloadEmptyWeapon();
	void UpdateAmmoValues();
	int32 AmountToReload();
	void HealMagnification(float DeltaTime);
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/************************************************
					 Crosshair
	*************************************************/
	void TraceUnderCrosshairs(FHitResult& TraceHitResult);
	void SetHUDCrosshairs(float DeltaTime);
	FHUDCrosshairStruct HUDCrosshairStruct;	
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;
	
	/************************************************
					HandyRefs
	*************************************************/
	UPROPERTY(VisibleAnywhere, Category = HandyRefs)
	APlayerCharacter* Character;
	UPROPERTY(VisibleAnywhere, Category = HandyRefs)
	ASneakyPlayerController* PC;
	UPROPERTY(VisibleAnywhere, Category = HandyRefs)
	ASneakyHUD* HUD;

	
	FVector HitTarget;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AWeapon* EquippedWeapon;
	UPROPERTY()
	AWeapon* SecondaryWeapon;
	/************************************************
					Ammo
	*************************************************/
	
	UPROPERTY(EditAnywhere)
	int32 StartingPistolAmmo = 5;
	// Carried ammo for the currently-equipped weapon
	UPROPERTY(EditAnywhere)
	int32 CarriedAmmo = 20;
	UPROPERTY(EditAnywhere)
	int32 MaxCarriedAmmo = 300;

	/************************************************
				FirstAid Healing
	*************************************************/
	bool bHealing = false;
	float HealingRate = 0.f;
	float AmountToHeal = 0.f;
};
