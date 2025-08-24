// Copyright Epic Games, Inc. All Rights Reserved.

#include "ParanoidGameMode.h"
#include "ParanoidCharacter.h"
#include "UObject/ConstructorHelpers.h"

AParanoidGameMode::AParanoidGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
