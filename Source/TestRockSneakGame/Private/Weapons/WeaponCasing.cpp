// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponCasing.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
AWeaponCasing::AWeaponCasing()
{
	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	SetRootComponent(CasingMesh);
	// Fix Camera Collision with the bullet
	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);

	// important
	CasingMesh->SetNotifyRigidBodyCollision(true);
	
	ShellEjectionImpulse = 10.f;
}

void AWeaponCasing::BeginPlay()
{
	Super::BeginPlay();

	// Binding
	CasingMesh->OnComponentHit.AddDynamic(this, &AWeaponCasing::OnHit);

	CasingMesh->AddImpulse(GetActorForwardVector() * ShellEjectionImpulse);
}

void AWeaponCasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if(ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
	}
	Destroy();
}

