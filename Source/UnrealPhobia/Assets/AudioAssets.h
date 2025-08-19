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
    TObjectPtr<USoundBase> BeingChased;

    UPROPERTY(EditAnywhere, Category = "Music")
    TObjectPtr<USoundBase> MainMenu;

    UPROPERTY(EditAnywhere, Category = "Creature")
    TObjectPtr<USoundBase> Growling;

    UPROPERTY(EditAnywhere, Category = "Creature")
    TObjectPtr<USoundBase> Scream;

    UPROPERTY(EditAnywhere, Category = "Survivor")
    TObjectPtr<USoundBase> WalkSteps;

    UPROPERTY(EditAnywhere, Category = "Survivor")
    TObjectPtr<USoundBase> RunSteps;

    UPROPERTY(EditAnywhere, Category = "Survivor")
    TObjectPtr<USoundBase> SwitchFlashLight;

    UPROPERTY(EditAnywhere, Category = "Player")
    TObjectPtr<USoundBase> BeDetected;

    /* Sound Classes */
    UPROPERTY(EditAnywhere, Category = "Audio Classes")
    TObjectPtr<USoundClass> MasterSoundClass;

    UPROPERTY(EditAnywhere, Category = "Audio Classes")
    TObjectPtr<USoundClass> MusicSoundClass;

    UPROPERTY(EditAnywhere, Category = "Audio Classes")
    TObjectPtr<USoundClass> SFXSoundClass;

    UPROPERTY(EditAnywhere, Category = "Audio Classes")
    TObjectPtr<USoundClass> UISoundClass;

    /* Global Sound Mix */
    UPROPERTY(EditAnywhere, Category = "AudioCategory")
    TObjectPtr<USoundMix> GlobalSoundMix;

private:
    static UAudioAssets* CachedAudioAssets;
};
