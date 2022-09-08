// Fill out your copyright notice in the Description page of Project Settings.


#include "SneakyPlayerController.h"

#include "PlayerCharacter.h"
#include "Components/CombatComponent.h"
#include "Pickups/AmmoPickup.h"
#include "Pickups/HealthPickup.h"
#include "UI/MainPlayerOverlay.h"
#include "UI/SneakyHUD.h"
#include "Weapons/Weapon.h"

class AHealthPickup;

void ASneakyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Action
	InputComponent->BindAction("StanceAction", IE_Pressed, this, &ASneakyPlayerController::OnStanceActionPressed);
	InputComponent->BindAction("Aim", IE_Pressed, this, &ASneakyPlayerController::OnAimButtonPressed);
	InputComponent->BindAction("Aim", IE_Released, this, &ASneakyPlayerController::OnAimButtonReleased);
	InputComponent->BindAction("Fire", IE_Pressed, this, &ASneakyPlayerController::OnFireButtonPressed);
	InputComponent->BindAction("Fire", IE_Released, this, &ASneakyPlayerController::OnFireButtonReleased);
	InputComponent->BindAction("SwapWeapons", IE_Pressed, this, &ASneakyPlayerController::OnSwapWeaponsPressed);
	InputComponent->BindAction("Pickup", IE_Pressed, this, &ASneakyPlayerController::PickupButtonPressed);
	InputComponent->BindAction("Reload", IE_Pressed, this, &ASneakyPlayerController::OnReloadButtonPressed);
	
	// Axis
	InputComponent->BindAxis("MoveForward/Backwards",	this, &ASneakyPlayerController::OnMoveForwardBackwards);
	InputComponent->BindAxis("MoveRight/Left",	this, &ASneakyPlayerController::OnMoveRightLeft);
	InputComponent->BindAxis("LookUp/Down",	this, &ASneakyPlayerController::OnLookUpDown);
	InputComponent->BindAxis("LookLeft/Right",	this, &ASneakyPlayerController::OnLookLeftRight);
}
void ASneakyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SneakyHUD = SneakyHUD == nullptr ? Cast<ASneakyHUD>(GetHUD()) : SneakyHUD;
	if(SneakyHUD)
	{
		if(SneakyHUD->MainPlayerOverlayRef == nullptr) SneakyHUD->AddCharacterOverlay();
	}
}
void ASneakyPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PlayerCharacter = Cast<APlayerCharacter>(InPawn);
}


void ASneakyPlayerController::OnStanceActionPressed()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->StanceActionInBPPressed();
	}
}

void ASneakyPlayerController::OnAimButtonPressed()
{
	if (PlayerCharacter == nullptr || PlayerCharacter->CombatComponent == nullptr) return;
	
	//CombatComponent->bAiming = true;
	PlayerCharacter->CombatComponent->SetAiming(true);
	PlayerCharacter->OnAimButtonPressedBP();
	
}

void ASneakyPlayerController::OnAimButtonReleased()
{
	if (PlayerCharacter == nullptr || PlayerCharacter->CombatComponent == nullptr) return;
	
	//CombatComponent->bAiming = false;
	PlayerCharacter->CombatComponent->SetAiming(false);
	PlayerCharacter->OnAimButtonReleasedBP();
	
}

void ASneakyPlayerController::OnFireButtonPressed()
{
	if (PlayerCharacter == nullptr || PlayerCharacter->CombatComponent == nullptr) return;
	

	PlayerCharacter->CombatComponent->FireButtonPressed(true);
	if (PlayerCharacter->CombatComponent->EquippedWeapon)
	{
		if (PlayerCharacter->CombatComponent->EquippedWeapon->GetWeaponType() == EWeaponType::ExpandableBaton)
		{
			
		}
		else
		{
			PlayerCharacter->OnAimButtonPressedBP();
		}
	}

}

void ASneakyPlayerController::OnFireButtonReleased()
{
	if (PlayerCharacter == nullptr || PlayerCharacter->CombatComponent == nullptr) return;

	PlayerCharacter->CombatComponent->FireButtonPressed(false);
}

void ASneakyPlayerController::OnSwapWeaponsPressed()
{
	if (PlayerCharacter == nullptr || PlayerCharacter->CombatComponent == nullptr) return;

	PlayerCharacter->CombatComponent->SwapWeapons();
}

void ASneakyPlayerController::PickupButtonPressed()
{
	if (PlayerCharacter == nullptr || PlayerCharacter->CombatComponent == nullptr || PlayerCharacter->OverlappingPickup == nullptr) return;
	{
		AAmmoPickup* AmmoPickup = Cast<AAmmoPickup>(PlayerCharacter->OverlappingPickup);
		if (AmmoPickup)
		{
			AmmoPickup->PickupAPickup();
			PlayerCharacter->OverlappingPickup = nullptr;
		}
		AHealthPickup* HealthPickup = Cast<AHealthPickup>(PlayerCharacter->OverlappingPickup);
		if (HealthPickup)
		{
			HealthPickup->PickupAPickup();
			PlayerCharacter->OverlappingPickup = nullptr;
		}
	}
}

void ASneakyPlayerController::OnReloadButtonPressed()
{
	if (PlayerCharacter == nullptr || PlayerCharacter->CombatComponent == nullptr) return;
	{
		PlayerCharacter->CombatComponent->Reload();
	}
}

void ASneakyPlayerController::OnMoveForwardBackwards(float AxisValue)
{
	if (PlayerCharacter)
	{
		if (AxisValue != 0.0f)
		{
			// find out which way is forward
			const FRotator Rotation = this->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			PlayerCharacter->AddMovementInput(Direction, AxisValue);
		}
	}
}

void ASneakyPlayerController::OnMoveRightLeft(float AxisValue)
{
	if (PlayerCharacter == nullptr) return;
	if(AxisValue != 0.f)
	{
		const FRotator YawRotation(0.f, this->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		PlayerCharacter->AddMovementInput(Direction, AxisValue);
	}
}

 void ASneakyPlayerController::OnLookUpDown(float AxisValue)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->AddControllerPitchInput(AxisValue);
	}
}

void ASneakyPlayerController::OnLookLeftRight(float AxisValue)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->AddControllerYawInput(AxisValue);
	}
}



void ASneakyPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	SneakyHUD = SneakyHUD == nullptr ? Cast<ASneakyHUD>(GetHUD()) : SneakyHUD;
	const bool bHUDValid = SneakyHUD && SneakyHUD->MainPlayerOverlayRef && SneakyHUD->MainPlayerOverlayRef->Txt_WeaponAmmoAmount;
	if(bHUDValid)
	{
		const FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		SneakyHUD->MainPlayerOverlayRef->Txt_WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void ASneakyPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	SneakyHUD = SneakyHUD == nullptr ? Cast<ASneakyHUD>(GetHUD()) : SneakyHUD;
	const bool bHUDValid = SneakyHUD && SneakyHUD->MainPlayerOverlayRef && SneakyHUD->MainPlayerOverlayRef->Txt_CarriedAmmoAmount;
	if(bHUDValid)
	{
		const FString CarriedAmmoText = FString::Printf(TEXT("%d"), Ammo);
		SneakyHUD->MainPlayerOverlayRef->Txt_CarriedAmmoAmount->SetText(FText::FromString(CarriedAmmoText));
	}
}

void ASneakyPlayerController::AmmoHorizontalBoxVisibility(ESlateVisibility InVisibility)
{
	SneakyHUD = SneakyHUD == nullptr ? Cast<ASneakyHUD>(GetHUD()) : SneakyHUD;
	const bool bHUDValid = SneakyHUD && SneakyHUD->MainPlayerOverlayRef && SneakyHUD->MainPlayerOverlayRef->AmmoHorizontalBox;
	if(bHUDValid)
	{
		SneakyHUD->MainPlayerOverlayRef->AmmoHorizontalBox->SetVisibility(InVisibility);
	}
}
