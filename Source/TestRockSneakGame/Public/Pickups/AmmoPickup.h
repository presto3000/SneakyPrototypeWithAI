// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Definitions.h"
#include "Pickups/Pickup.h"
#include "AmmoPickup.generated.h"

/**
 * 
 */
UCLASS()
class TESTROCKSNEAKGAME_API AAmmoPickup : public APickup
{
	GENERATED_BODY()

public:
	void PickupAmmo();
	virtual void PickupAPickup() override;
protected:

private:
	UPROPERTY(EditAnywhere)
	int32 AmmoAmount = 30;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;
};
