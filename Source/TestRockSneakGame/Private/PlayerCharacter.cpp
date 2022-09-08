// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "MyGameModeBase.h"
#include "SneakyPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/CombatComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Pickups/Pickup.h"
#include "Sound/SoundCue.h"
#include "Weapons/Weapon.h"


APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);
	
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if(Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		Destroy();

		UWorld* World = GetWorld();
		if(World)
		{
			UKismetSystemLibrary::ExecuteConsoleCommand(World, TEXT("RestartLevel"));
		}
	}
	else
	{
		Health -= DamageAmount;
	}
	return DamageAmount;
}

void APlayerCharacter::PostInitializeComponents()
{

	Super::PostInitializeComponents();
	if (CombatComponent)
	{
		CombatComponent->Character = this;
	}
}

void APlayerCharacter::BulletHitOnCharacter(FHitResult FireHit)
{
	if(ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if(ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FireHit.Location, FRotator(0.f), true);
	}
}

void APlayerCharacter::SetOverlappingPickup(APickup* Pickup)
{
	OverlappingPickup = Pickup;
	if(OverlappingPickup)
	{
		OverlappingPickup->ShowPickupWidget(true);
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	SpawnDefaultWeapon();
	SpawnSecondaryWeapon();

	UpdateHUDAmmo();
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void APlayerCharacter::PlayFireMontage()
{
	if(CombatComponent == nullptr || CombatComponent->EquippedWeapon == nullptr) return;
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if(AnimInstance && FireWeaponMontage)
		{
			AnimInstance->Montage_Play(FireWeaponMontage);
		}
	}
}

void APlayerCharacter::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && IsValid(DefaultWeaponClass))
	{
		AWeapon* StartingWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		StartingWeapon->bDestroyWeapon = true;
		if (CombatComponent)
		{
			CombatComponent->EquipWeapon(StartingWeapon);
		}
	}
}
void APlayerCharacter::SpawnSecondaryWeapon()
{
	UWorld* World = GetWorld();
	if (World && IsValid(SecondaryWeaponClass))
	{
		AWeapon* SecondaryWeapon = World->SpawnActor<AWeapon>(SecondaryWeaponClass);
		SecondaryWeapon->bDestroyWeapon = true;
		if (CombatComponent)
		{
			CombatComponent->EquipWeapon(SecondaryWeapon);
		}
	}
}
void APlayerCharacter::PlayReloadMontage()
{
	//if(CombatComponent == nullptr || CombatComponent->EquippedWeapon == nullptr) return;
	//{
	//	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	//	if(AnimInstance && ReloadMontage)
	//	{
	//		AnimInstance->Montage_Play(ReloadMontage);
//
	//	}
	//}
}

void APlayerCharacter::UpdateHUDAmmo()
{
	SneakyPlayerController = SneakyPlayerController == nullptr ? Cast<ASneakyPlayerController>(Controller) : SneakyPlayerController;
	if(SneakyPlayerController && CombatComponent && CombatComponent->EquippedWeapon)
	{
		SneakyPlayerController->SetHUDCarriedAmmo(CombatComponent->CarriedAmmo);
		SneakyPlayerController->SetHUDWeaponAmmo(CombatComponent->EquippedWeapon->GetAmmo());
	}
}
