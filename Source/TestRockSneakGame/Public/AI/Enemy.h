// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICombatComponent.h"
#include "GameFramework/Character.h"
#include "Interfaces/InteractWithCrosshairs.h"
#include "Enemy.generated.h"

class AWeapon;
class UCombatComponent;
class AEnemyController;
UCLASS()
class TESTROCKSNEAKGAME_API AEnemy : public ACharacter, public IInteractWithCrosshairs
{
	GENERATED_BODY()

public:
	AEnemy(FObjectInitializer const& ObjectInitializer = FObjectInitializer::Get());
	UPROPERTY(VisibleAnywhere, Category = HandyRefs)
	AEnemy* Character;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HandyRefs)
	bool bEnemy = true;

	UFUNCTION(BlueprintNativeEvent)
	void Death();
protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere)
	UAICombatComponent* AICombatComponent;
	void SpawnDefaultWeapon();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AWeapon* StartingWeapon;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;
	
	/** Particles to spawn when hit by bullets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ImpactParticles;

	/** Sound to play when hit by bullets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* ImpactSound;
	
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void BulletHit_Implementation(FHitResult HitResult) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintNativeEvent)
	void ShowHealthBar();
	void ShowHealthBar_Implementation();
	/** In BP */
	UFUNCTION(BlueprintImplementableEvent)
	void HideHealthBar();
	/** Time to display health bar once shot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HealthBarDisplayTime = 4.f;
	FTimerHandle HealthBarTimer;
	
private:

	/** Current Enemy Health */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float Health = 100.f;
	/** Max Enemy Health */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float BaseDamage = 40.f;
	
	/*****************************************
	 *				Behaviour Tree
	 *****************************************/
	/** Behavior tree for the AI Character */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviorTree;

	// Pushing Vector values in Editor to BT
	/** Point 1 for the enemy to move to */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint;
	/** Point 2 for the enemy to move to */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint2;
	/** Point 3 for the enemy to move to */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint3;

	UPROPERTY()
	AEnemyController* EnemyController;

public:
	FORCEINLINE UBehaviorTree* GetBehaviorTree() const {return BehaviorTree; }
	
};
