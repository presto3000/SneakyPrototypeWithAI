
#pragma once

#define	TRACE_LENGTH 50000.f

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	Unoccupied UMETA(DisplayName = "Unoccupied"),
	Reloading UMETA(DisplayName = "Reloading"),
	Melee UMETA(DisplayName = "Melee"),
	Stunned UMETA(DisplayName = "Stunned"),
};



UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	AssaultRifle UMETA(DisplayName = "AssaultRifle"),
	Pistol UMETA(DisplayName = "Pistol"),
	ExpandableBaton UMETA(DisplayName = "ExpandableBaton"),
};