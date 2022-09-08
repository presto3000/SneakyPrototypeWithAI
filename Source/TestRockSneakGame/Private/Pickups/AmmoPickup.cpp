// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/AmmoPickup.h"
#include "PlayerCharacter.h"
#include "Components/CombatComponent.h"

void AAmmoPickup::PickupAmmo()
{
	if(PlayerCharacter)
	{
		UCombatComponent* CombatComponent = PlayerCharacter->CombatComponent;
		if(CombatComponent)
		{
			CombatComponent->PickupAmmo(WeaponType, AmmoAmount);
		}
	}
	Destroy();
}

void AAmmoPickup::PickupAPickup()
{
	if(PlayerCharacter)
	{
		UCombatComponent* CombatComponent = PlayerCharacter->CombatComponent;
		if(CombatComponent)
		{
			CombatComponent->PickupAmmo(WeaponType, AmmoAmount);
		}
	}
	Super::PickupAPickup();
}

