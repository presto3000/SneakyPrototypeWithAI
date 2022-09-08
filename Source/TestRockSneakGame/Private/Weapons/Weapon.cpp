// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Weapon.h"
#include "DrawDebugHelpers.h"
#include "PlayerCharacter.h"
#include "SneakyPlayerController.h"
#include "AI/Enemy.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Interfaces/InteractWithCrosshairs.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LevelActors/AmmoHealthBox.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Weapons/WeaponCasing.h"
#include "Perception/AISense_Hearing.h"

// Sets default values
AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
	SetRootComponent(WeaponMesh);
	
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

}

void AWeapon::Fire(const FVector& HitTarget, bool OwnerAI)
{
	// Fire Animation
	if(FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
	
	// Spawn Bullets
	if(CasingClass)
	{
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
		if(AmmoEjectSocket)
		{
			const FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(GetWeaponMesh());
			
			// Character owning the weapon
			UWorld* World = GetWorld();
			if(World)
			{
				World->SpawnActor<AWeaponCasing>(CasingClass, SocketTransform.GetLocation(),  SocketTransform.GetRotation().Rotator());
			}
		}
	}
	
	// Spend Round
	SpendRound();
	
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if(OwnerPawn == nullptr) return;

	/////////////////AI//////////////////////////////////////////////////////////
	//FVector EyeLocation;
	//FRotator EyeRotation;
	//OwnerPawn->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	//FVector ShotDirection = EyeRotation.Vector();
	//float HalfRad = FMath::DegreesToRadians(BulletSpread);
	//ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);
	//FVector TraceEnd = EyeLocation + (ShotDirection * TRACE_LENGTH);
	/////////////////////////////////////////////////////////////////////////////
	
	AController* InstigatorController = OwnerPawn->GetController();

	// Get Socket Transform
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if(MuzzleFlashSocket && InstigatorController)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();

		//// Weapon TraceHIt if we are the Player ***
		FHitResult FireHit;
		//if (!OwnerAI)
		//{
			WeaponTraceHit(Start, HitTarget, FireHit);
		//}
		//// AI ***
		//else
		//{
		//	WeaponTraceHit(Start, HitTarget, FireHit);
		//	//DrawDebugLine(GetWorld(), Start, FVector(HitTarget),
		//	//FColor::Purple, false, 2.f);
		//}

		//*****//

		// Does hit Actor implement InteractWithCrosshairs Interface?
		if (FireHit.Actor.IsValid())
		{
			// Check if Actor inherits from Interface
			IInteractWithCrosshairs* InteractWithCrosshairsInterface = Cast<IInteractWithCrosshairs>(FireHit.Actor.Get());
			if (InteractWithCrosshairsInterface)
			{
				InteractWithCrosshairsInterface->BulletHit_Implementation(FireHit);
		

				AEnemy* HitEnemy = Cast<AEnemy>(FireHit.Actor.Get());
				// FireHit.Actor is an Actor with TakeDamage func, basically a Box
				AAmmoHealthBox* AmmoHealthBox  = Cast<AAmmoHealthBox>(FireHit.Actor.Get());
				if (HitEnemy)
				{
					// Apply Damage
					UGameplayStatics::ApplyDamage(FireHit.Actor.Get(), Damage, InstigatorController, this, UDamageType::StaticClass());
				}
				else if (AmmoHealthBox != nullptr)
				{
					// Apply Damage
					UGameplayStatics::ApplyDamage(FireHit.Actor.Get(), Damage, InstigatorController, this, UDamageType::StaticClass());
				}
				else
				{
					// Spawn Particles if FirehitResult is not a valid Actor
					if(ImpactParticles)
					{
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FireHit.ImpactPoint, FireHit.ImpactNormal.Rotation());
					}
					// Play Impact Sound
					if(HitSound)
					{
						UGameplayStatics::PlaySoundAtLocation(this, HitSound, FireHit.ImpactPoint);
						UAISense_Hearing::ReportNoiseEvent(this, FireHit.ImpactPoint, 2, InstigatorController, 1000, TEXT("Noise"));
					}
				}
			}
			else
			{
				APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(FireHit.Actor.Get());
				if (PlayerCharacter)
				{
					// Apply Damage
					UGameplayStatics::ApplyDamage(FireHit.Actor.Get(), Damage, InstigatorController, this, UDamageType::StaticClass());
					PlayerCharacter->BulletHitOnCharacter(FireHit);
				}
				else
				{
					// Spawn Particles if FirehitResult is not a valid Actor
					if(ImpactParticles)
					{
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FireHit.ImpactPoint, FireHit.ImpactNormal.Rotation());
					}
					// Play Impact Sound
					if(HitSound)
					{
						UGameplayStatics::PlaySoundAtLocation(this, HitSound, FireHit.ImpactPoint);
						UAISense_Hearing::ReportNoiseEvent(this, FireHit.ImpactPoint, 1, InstigatorController, 2000, TEXT("Noise"));
					}
				}
			}
		}
		
		// Muzzle Flash Emitter
		if(MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
			
		}
		
		// FireSound
		if(FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			//UAISense_Hearing::ReportNoiseEvent(this, GetActorLocation(), 1, InstigatorController, 2000, TEXT("Noise"));
		}

	}
//
	
}
void AWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
	const AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		
		const FVector End = TraceStart + (HitTarget - TraceStart) * 1.25f;
		const UWorld* World = GetWorld();
	

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
	
		World->LineTraceSingleByChannel(OutHit, TraceStart, End, ECollisionChannel::ECC_Visibility, QueryParams);
	
		FVector BeamEnd = End;
		if(OutHit.bBlockingHit)
		{
			BeamEnd = OutHit.ImpactPoint;
		}
		DrawDebugSphere(GetWorld(), BeamEnd, 16.f, 12, FColor::Orange, false, 2);
		if(BeamParticles)
		{
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(World, BeamParticles, TraceStart, FRotator::ZeroRotator);
			if(Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
		//DrawDebugLine(GetWorld(), TraceStart, FVector(BeamEnd),
		//FColor::Cyan, false, 2.f);
	}
	
}
FVector AWeapon::TraceEndWithScatter(const FVector& HitTarget)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if(MuzzleFlashSocket == nullptr) return FVector();
	
	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	const FVector TraceStart = SocketTransform.GetLocation();


	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;

	const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
	const FVector EndLoc = SphereCenter + RandVec;
	const FVector ToEndLoc = EndLoc - TraceStart;

	//DrawDebugSphere(GetWorld(), SphereCenter, SphereRadius, 12, FColor::Red, false, 2.f);
	DrawDebugSphere(GetWorld(), ToEndLoc, 4.f, 12, FColor::Orange, false, 2.f);
	
	//DrawDebugLine(GetWorld(), TraceStart, FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size()),
	//	FColor::Cyan, true);
	
	return FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size());
}
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AWeapon::AddAmmo(int32 AmmoToAdd)
{
	Ammo = FMath::Clamp(Ammo + AmmoToAdd, 0, MagCapacity);
	SetHUDAmmo();
}
void AWeapon::SpendRound()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
	SetHUDAmmo();
}
bool AWeapon::IsEmpty()
{
	return Ammo <= 0;
}

bool AWeapon::IsFull()
{
	return Ammo == MagCapacity;
}
void AWeapon::SetHUDAmmo()
{
	Character = Character == nullptr ? Cast<APlayerCharacter>(GetOwner()) : Character;
	if(Character)
	{
		//Cast<ASneakyPlayerController>(Character->Controller)
		PC = PC == nullptr ? Cast<ASneakyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)) : PC;
		if(PC)
		{
			PC->SetHUDWeaponAmmo(Ammo);
		}
	}
}
void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	OnWeaponStateSet();
}

void AWeapon::OnWeaponStateSet()
{
	switch (WeaponState)
	{
	case EWeaponState::Equipped:
		break;
	case EWeaponState::EquippedSecondary:
		break;
	}
}
