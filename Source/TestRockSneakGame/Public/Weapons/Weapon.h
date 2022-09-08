// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Definitions.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class ASneakyPlayerController;
class APlayerCharacter;
class AWeaponCasing;
class USoundCue;


UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Initial UMETA(DisplayName = "Inital State"),
	Equipped UMETA(DisplayName = "Equipped State"),
	EquippedSecondary UMETA(DisplayName = "Equipped Secondary"),
};

UCLASS()
class TESTROCKSNEAKGAME_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();

	UPROPERTY(BlueprintReadWrite, Category = HandyRefs)
	APlayerCharacter* Character;
	UPROPERTY(BlueprintReadWrite, Category = HandyRefs)
	ASneakyPlayerController* PC;
	/************************************************
					FUNCTIONS
	*************************************************/
	void Fire(const FVector& HitTarget, bool OwnerAI);
	void SpendRound();
	void AddAmmo(int32 AmmoToAdd);
	bool IsEmpty();
	bool IsFull();
	void SetWeaponState(EWeaponState State);
	void OnWeaponStateSet();


	/************************************************
						HUD
	*************************************************/
	void SetHUDAmmo();
protected:
	
	virtual void BeginPlay() override;
	void WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit);

public:	
	virtual void Tick(float DeltaTime) override;
	bool bDestroyWeapon = false;

	/************************************************
					Weapon Scatter
	*************************************************/
	FVector TraceEndWithScatter(const FVector& HitTarget);
	UPROPERTY(EditAnywhere, Category = WeaponScatter)
	float DistanceToSphere = 800.f;
	UPROPERTY(EditAnywhere, Category = WeaponScatter)
	float SphereRadius = 75.f;
	UPROPERTY(EditAnywhere, Category = WeaponScatter)
	bool bUseScatter = false;

	/************************************************
					 Crosshair
	*************************************************/
	// Crosshair : Textures for the weapon crosshairs
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsCenter;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsLeft;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsRight;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsBottom;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsTop;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;
	UPROPERTY(VisibleAnywhere, Category = WeaponProperties)
	EWeaponState WeaponState;

	
	UPROPERTY(VisibleAnywhere, Category = WeaponProperties)
	USkeletalMeshComponent* WeaponMesh;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeaponCasing> CasingClass;


	UPROPERTY(EditAnywhere)
	int32 Ammo;
	UPROPERTY(EditAnywhere)
	int32 MagCapacity;
	UPROPERTY(EditAnywhere)
	float Damage = 30.f;
	/************************************************
						 Fire Effects
	*************************************************/
	UPROPERTY(EditAnywhere)
	UParticleSystem* BeamParticles;
	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleFlash;
	UPROPERTY(EditAnywhere)
	USoundCue* FireSound;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles;
	UPROPERTY(EditAnywhere)
	USoundCue* HitSound;
	UPROPERTY(EditAnywhere, Category = WeaponProperties)
	UAnimationAsset* FireAnimation;


	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin=0.0f))
	float BulletSpread = 2.f;


public:
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const {return WeaponMesh; }
	FORCEINLINE EWeaponType GetWeaponType() const {return WeaponType; }
	FORCEINLINE int32 GetAmmo() const {return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const {return MagCapacity; }
	FORCEINLINE float GetDamage() const {return Damage; }
};
