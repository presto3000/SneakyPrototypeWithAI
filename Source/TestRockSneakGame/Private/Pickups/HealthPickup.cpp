// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/HealthPickup.h"
#include "Components/CombatComponent.h"


void AHealthPickup::PickupAPickup()
{
	if(PlayerCharacter)
	{
		UCombatComponent* CombatComponent = PlayerCharacter->CombatComponent;
		if(CombatComponent)
		{
			CombatComponent->Heal(HealAmount, HealingTime);
		}
	}
	Super::PickupAPickup();

}
