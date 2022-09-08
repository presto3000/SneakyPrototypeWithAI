// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class ASneakyPlayerController;
class APickup;
class USoundCue;
class UWidgetComponent;
class UCombatComponent;
class AWeapon;
UCLASS()
class TESTROCKSNEAKGAME_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	// Take combat damage
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void PostInitializeComponents() override;

	void PlayFireMontage();
	void PlayReloadMontage();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCombatComponent* CombatComponent;
	/************************************************
				Input BP Callbacks
	*************************************************/

	UFUNCTION(BlueprintImplementableEvent)
	void StanceActionInBPPressed();
	UFUNCTION(BlueprintImplementableEvent)
	void OnAimButtonPressedBP();
	UFUNCTION(BlueprintImplementableEvent)
	void OnAimButtonReleasedBP();
		
	/** Current MainCharacter Health */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float Health = 100.f;
	/** Max MainCharacter Health */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 100.f;
	
	void BulletHitOnCharacter(FHitResult FireHit);
	UPROPERTY()
	APickup* OverlappingPickup;
	void SetOverlappingPickup(APickup* Pickup);


	UPROPERTY()
	ASneakyPlayerController* SneakyPlayerController;

	void UpdateHUDAmmo();
protected:
	virtual void BeginPlay() override;
	
	void SpawnDefaultWeapon();
	void SpawnSecondaryWeapon();
	/************************************************
						 Default Weapon and 2ndary Weapon
	*************************************************/
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> SecondaryWeaponClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* OverheadWidget;

	/************************************************
						 Animation Montages
	*************************************************/
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* FireWeaponMontage;
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ReloadMontage;
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ElimMontage;


	/** Particles to spawn when hit by bullets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;
	/** Sound to play when hit by bullets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USoundCue* ImpactSound;
	
public:	
	virtual void Tick(float DeltaTime) override;



};
