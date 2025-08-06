#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"

#include "AudioAssets.generated.h"

/**
*           Name				: AudioAssets
*           Description		: Manage audio sources that used in this game
*           LastUpdate	    : 2025/08/06, Implement LoadAudioAssets()
*/

UCLASS(Blueprintable)
class UNREALPHOBIA_API UAudioAssets : public UDataAsset
{
    GENERATED_BODY()

public:
        
    /* Load AudioAssets to Target Classes*/
    UFUNCTION(BlueprintCallable, Category = "Audio")
    static UAudioAssets* LoadAudioAssets();

    /* Audio Sources */
    UPROPERTY(EditAnywhere,  Category = "Music")
    USoundBase* BeingChased;

    UPROPERTY(EditAnywhere, Category = "Music")
    USoundBase* MainMenu;

    UPROPERTY(EditAnywhere, Category = "Creature")
    USoundBase *Growling;

    UPROPERTY(EditAnywhere, Category = "Survivor")
    USoundBase *WalkingSteps;

    UPROPERTY(EditAnywhere, Category = "Survivor")
    USoundBase *SwitchFlashLight;

    UPROPERTY(EditAnywhere, Category = "Survivor")
    USoundBase* HandLightButton;

    UPROPERTY(EditAnywhere, Category = "Player")
    USoundBase* BeDetected;

    /* Sound Classes */
    UPROPERTY(EditAnywhere, Category = "Audio Classes")
    USoundClass* MusicSoundClass;

    UPROPERTY(EditAnywhere, Category = "Audio Classes")
    USoundClass* SFXSoundClass;

    UPROPERTY(EditAnywhere, Category = "Audio Classes")
    USoundClass* UISoundClass;


    /* Global Sound Mix */
    UPROPERTY(EditAnywhere, Category = "AudioCategory")
    USoundMix* GlobalSoundMix;

private:
    static UAudioAssets* CachedAudioAssets;
};
