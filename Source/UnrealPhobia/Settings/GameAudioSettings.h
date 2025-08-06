#pragma once

#include "CoreMinimal.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/GameUserSettings.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"

#include "Assets/AudioAssets.h"
#include "Types/GameAudioTypes.h"

#include "GameAudioSettings.generated.h"

/**
*           Name				: GameAudioSettings
*           Description		: Environment Setting of Audio 
*           LastUpdate	    : 2025/08/06, Implement volume control system
*/

class UAudioAssets;

UCLASS()
class UNREALPHOBIA_API UGameAudioSettings : public UGameUserSettings
{
    GENERATED_BODY()

public:
    UGameAudioSettings();

    /* Static Accessor */
    UFUNCTION(BlueprintCallable, Category = "Settings")
    static UGameAudioSettings* GetGameAudioSettings();

    /* Set Audio Volumes */
    UFUNCTION(BlueprintCallable, Category = "Setting Audio Volume")
    void SetMasterVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Setting Audio Volume")
    void SetSFXVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Setting Audio Volume")
    void SetMusicVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Setting Audio Volume")
    void SetUIVolume(float Volume);

    /* Get Audio Volumes */
    UFUNCTION(BlueprintPure, Category = "Getting Audio Volume")
    float GetMasterVolume() const { return MasterVolume; };

    UFUNCTION(BlueprintPure, Category = "Getting Audio Volume")
    float GetSFXVolume() const { return SFXVolume; };

    UFUNCTION(BlueprintPure, Category = "Getting Audio Volume")
    float GetMusicVolume() const { return MusicVolume; };

    UFUNCTION(BlueprintPure, Category = "Getting Audio Volume")
    float GetUIVolume() const { return UIVolume; };


protected:
    /* Volume Values */
    UPROPERTY(Config)
    float MasterVolume;

    UPROPERTY(Config)
    float MusicVolume;

    UPROPERTY(Config)
    float SFXVolume;

    UPROPERTY(Config)
    float UIVolume;

private:
    UAudioAssets* AudioAssets;

    // 모든 오디오 설정 적용
    void ApplyAudioSettings();
    // 모든 오디오 설정 초기화
    void ResetAudioToDefaults();
    // 유효성 검사
    bool IsAudioAssetsReady(USoundClass* SoundClass) const;
    // SoundMix에 볼륨 적용
    void ApplySoundMixOverride(USoundClass* SoundClass, float Volume);
};
