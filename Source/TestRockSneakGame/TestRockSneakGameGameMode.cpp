// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestRockSneakGameGameMode.h"
#include "TestRockSneakGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATestRockSneakGameGameMode::ATestRockSneakGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
