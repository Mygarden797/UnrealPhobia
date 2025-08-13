#pragma once

#include "CoreMinimal.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Types/GameAudioTypes.h"
#include "GameAudioSettings.generated.h"

/**
*           Name				: GameAudioSettings
*           Description		: Environment Setting of Audio 
*           LastUpdate	    : 2025/08/06, Implement volume control system
*/

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAudioSettingsChanged);

class UAudioAssets;

UCLASS()
class UNREALPHOBIA_API UGameAudioSettings : public UGameUserSettings
{
    GENERATED_BODY()

public:
    UGameAudioSettings();

    UFUNCTION(BlueprintCallable, Category = "Settings")
    static UGameAudioSettings* GetGameAudioSettings();

    /* Set Audio Volumes */
    UFUNCTION(BlueprintCallable, Category = "Setting Audio Volume")
    void SetVolume(EAudioCategory Category, float Value);

    /* Get Audio Volumes */
    UFUNCTION(BlueprintPure, Category = "Getting Audio Volume")
    float GetVolume(EAudioCategory Category) const;


    /* 모든 오디오 설정 초기화 */
    // virtual void SetToDefaults() override;
    void ResetAudioToDefaultsOnly();

    UPROPERTY(BlueprintAssignable, Category = "Game Audio Settings")
    FOnAudioSettingsChanged OnAudioSettingsChanged;

protected:
    /* Volume Values */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Audio")
    float MasterVolume;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Audio")
    float MusicVolume;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Audio")
    float SFXVolume;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Audio")
    float UIVolume;
};

