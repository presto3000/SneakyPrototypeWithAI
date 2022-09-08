// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "MainPlayerOverlay.generated.h"

/**
 * 
 */
UCLASS()
class TESTROCKSNEAKGAME_API UMainPlayerOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_WeaponAmmoAmount;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_CarriedAmmoAmount;
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* AmmoHorizontalBox;
};
