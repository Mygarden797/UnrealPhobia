#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Sound/SoundBase.h"
#include "AudioAssets.generated.h"

/*
    - Name				: AudioAssets
    - Description		:  Manage audio sources that used in this game
    - Date				    : 2025/07/28
*/

UCLASS(Blueprintable)
class UNREALPHOBIA_API UAudioAssets : public UDataAsset
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere,  Category = "BGM")
    USoundBase* BeingChased;

    UPROPERTY(EditAnywhere, Category = "BGM")
    USoundBase* MainMenu;

    UPROPERTY(EditAnywhere,  Category = "Creature")
    USoundBase* Growling;

    UPROPERTY(EditAnywhere,  Category = "Survivor")
    USoundBase* WalkingSteps;

    UPROPERTY(EditAnywhere, Category = "Player")
    USoundBase* Detected;
};
