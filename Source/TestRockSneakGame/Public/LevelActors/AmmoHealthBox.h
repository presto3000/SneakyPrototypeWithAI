// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractWithCrosshairs.h"
#include "AmmoHealthBox.generated.h"

UCLASS()
class TESTROCKSNEAKGAME_API AAmmoHealthBox : public AActor, public IInteractWithCrosshairs
{
	GENERATED_BODY()
	
public:	

	AAmmoHealthBox();
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
protected:

	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Health = 100;

};
