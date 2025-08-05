#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "Types/GameAudioTypes.h"
#include "GameAudioSettings.generated.h"

/**
 *
 */

UCLASS()
class UNREALPHOBIA_API UGameAudioSettings : public UGameUserSettings
{
    GENERATED_BODY()

public:
    UGameAudioSettings();

    virtual void ApplySettings(bool bCheckForCommandLineOverrides) override;
    virtual void SetToDefaults() override;

    /* Static Accessor */
    UFUNCTION(BlueprintCallable, Category = "Settings")
    static UGameAudioSettings* GetGameAudioSettings();

    /* Set Audio Volumes */
    UFUNCTION(BlueprintCallable, Category = "Settings Audio Volume")
    void SetCategoryVolume(EAudioCategory Category, float Volume);

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
    float GetCategoryVolume(EAudioCategory Category) const;

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
    float MasterVolume = 1.0f;

    UPROPERTY(Config)
    float MusicVolume = 1.0f;

    UPROPERTY(Config)
    float SFXVolume = 1.0f;

    UPROPERTY(Config)
    float UIVolume = 1.0f;

private:
    void ApplyAudioSettings();
    void ClampVolume(float& Volume);

};
