// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Enemy.h"

#include "DrawDebugHelpers.h"
#include "AI/EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"
#include "Weapons/Weapon.h"

AEnemy::AEnemy(FObjectInitializer const& ObjectInitializer)
{
	AICombatComponent = ObjectInitializer.CreateDefaultSubobject<UAICombatComponent>(this, TEXT("AICombatComponent2"));
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemy::Death_Implementation()
{
	// Hide HealthBar
	HideHealthBar();
}

void AEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (AICombatComponent)
	{
		AICombatComponent->Character = this;
	}
}
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	
	// Visibility channel set to Block like line trace
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	// World Position for LocalPositions of Patrol Points
	const FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint);
	const FVector WorldPatrolPoint2 = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint2);
	const FVector WorldPatrolPoint3 = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint3);

	//Debug helpers:
	DrawDebugSphere(GetWorld(),
		WorldPatrolPoint,
		25.f,
		12,
		FColor::Red,
		true);
	//Debug helpers:
	DrawDebugSphere(GetWorld(),
		WorldPatrolPoint2,
		25.f,
		12,
		FColor::Red,
		true);
	//Debug helpers:
	DrawDebugSphere(GetWorld(),
		WorldPatrolPoint3,
		25.f,
		12,
		FColor::Red,
		true);

	// Get the AI Controller
	EnemyController = Cast<AEnemyController>(GetController());
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WorldPatrolPoint);
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint2"), WorldPatrolPoint2);
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint3"), WorldPatrolPoint3);
		// Run BT
		EnemyController->RunBehaviorTree(BehaviorTree);
	}



	// Spawn Default Weapon for AI
	//SpawnDefaultWeapon();
}

void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && IsValid(DefaultWeaponClass))
	{
		StartingWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy::BulletHit_Implementation(FHitResult HitResult)
{
	IInteractWithCrosshairs::BulletHit_Implementation(HitResult);

	if(ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if(ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, HitResult.Location, FRotator(0.f), true);
	}
	// Show HealthBar
	ShowHealthBar();
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if(Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		Death();
	}
	else
	{
		Health -= DamageAmount;
	}
	return  DamageAmount;

}

void AEnemy::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	GetWorldTimerManager().SetTimer(
		HealthBarTimer,
		this,
		&AEnemy::HideHealthBar,
		HealthBarDisplayTime);
}