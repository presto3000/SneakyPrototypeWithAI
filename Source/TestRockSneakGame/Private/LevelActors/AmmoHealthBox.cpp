// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelActors/AmmoHealthBox.h"

AAmmoHealthBox::AAmmoHealthBox()
{
}

float AAmmoHealthBox::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if(Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		Destroy();
	}
	else
	{
		Health -= DamageAmount;
	}
	return  DamageAmount;
}

// Called when the game starts or when spawned
void AAmmoHealthBox::BeginPlay()
{
	Super::BeginPlay();
	
}


