// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/Pickup.h"
#include "HealthPickup.generated.h"

/**
 * 
 */
UCLASS()
class TESTROCKSNEAKGAME_API AHealthPickup : public APickup
{
	GENERATED_BODY()
public:

	virtual void PickupAPickup() override;
private:
	UPROPERTY(EditAnywhere)
	float HealAmount = 100.f;
	
	UPROPERTY(EditAnywhere)
	float HealingTime = 4.f;
};
