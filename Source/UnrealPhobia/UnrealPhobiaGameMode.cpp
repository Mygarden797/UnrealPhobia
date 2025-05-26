// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealPhobiaGameMode.h"
#include "UnrealPhobiaCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUnrealPhobiaGameMode::AUnrealPhobiaGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter_Trigger"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
