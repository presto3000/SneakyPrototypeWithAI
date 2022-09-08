// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Definitions.h"
#include "Components/ActorComponent.h"
#include "AICombatComponent.generated.h"


class AEnemyController;
class AEnemy;
class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TESTROCKSNEAKGAME_API UAICombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	// Sets default values for this component's properties
	UAICombatComponent();
	UFUNCTION(BlueprintCallable)
	void EquipWeapon(AWeapon* WeaponToEquip);
	UFUNCTION(BlueprintCallable)
	void FireButtonPressed(bool bPressed);
	void SetAiming(bool bIsAiming);
	void SwapWeapons();
	void Reload();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ECombatState CombatState = ECombatState::Unoccupied;
	
protected:
	virtual void BeginPlay() override;
	bool bAiming = false;
	bool bFireButtonPressed = false;
	bool bCanFire = true;
	void Fire();
	void EquipPrimaryWeapon(AWeapon* WeaponToEquip);
	void EquipSecondaryWeapon(AWeapon* WeaponToEquip);
	
	void AttachActorToRightHand(AActor* ActorToAttach);
	void AttachActorToRightPelvis(AActor* ActorToAttach);

	void ReloadEmptyWeapon();
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	/************************************************
					HandyRefs
	*************************************************/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HandyRefs)
	AEnemy* Character;
	UPROPERTY(VisibleAnywhere, Category = HandyRefs)
	AEnemyController* PC;

	
	FVector HitTarget;
	UPROPERTY()
	AWeapon* EquippedWeapon;
	UPROPERTY()
	AWeapon* SecondaryWeapon;

	
	UPROPERTY(EditAnywhere)
	int32 StartingPistolAmmo = 0;
	// Carried ammo for the currently-equipped weapon
	UPROPERTY(EditAnywhere)
	int32 CarriedAmmo;

	/***********************************
	 *				AI Fire
	 ************************************/
	UFUNCTION(BlueprintCallable)
	void AIStartFire();
	UFUNCTION(BlueprintCallable)
	void AIStopFire();
	FTimerHandle TimerHandle_TimeBetweenShots;
	float LastFireTime;
	// Derived from RateOfFire
	float TimeBetweenShots;
	float RateOfFire = 600.f;
};
