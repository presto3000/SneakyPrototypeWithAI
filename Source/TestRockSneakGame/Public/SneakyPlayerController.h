// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SneakyPlayerController.generated.h"

class ASneakyHUD;
class APlayerCharacter;
/**
 * 
 */
UCLASS()
class TESTROCKSNEAKGAME_API ASneakyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY()
	APlayerCharacter* PlayerCharacter;
	void AmmoHorizontalBoxVisibility(ESlateVisibility InVisibility);
	
	/********************************************************************
				Input callback functions
	********************************************************************/

	/************************************************
					 ACTIONS
	*************************************************/
	
	void OnStanceActionPressed();

	void OnAimButtonPressed();
	void OnAimButtonReleased();
	void OnFireButtonPressed();
	void OnFireButtonReleased();
	
	void OnSwapWeaponsPressed();
	void PickupButtonPressed();
	
	void OnReloadButtonPressed();
	
	/************************************************
					 AXIS
	*************************************************/
	UFUNCTION()
	void OnMoveForwardBackwards(float AxisValue);
	UFUNCTION()
	void OnMoveRightLeft(float AxisValue);
	UFUNCTION()
	void OnLookUpDown(float AxisValue);
	UFUNCTION()
	void OnLookLeftRight(float AxisValue);

	/************************************************
					HUD
	*************************************************/
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	ASneakyHUD* SneakyHUD;

};




