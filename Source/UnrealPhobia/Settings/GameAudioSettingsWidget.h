#pragma once

#include "CoreMinimal.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Types/GameAudioTypes.h"
#include "Settings/GameSettingsWidget.h"
#include "GameAudioSettingsWidget.generated.h"

/**
*           Name				: GameSettingsWidget
*           Description		: Widget of Audio Settings
*           LastUpdate	    : 2025/08/10
*           
*/

UCLASS()
class UNREALPHOBIA_API UGameAudioSettingsWidget : public UGameSettingsWidget
{
	GENERATED_BODY()
	
public:
    /* Constructor */
    virtual void NativeConstruct() override;

protected:
    /* GameAudioSettings' Override Functions */
    // GameAudioSettings에서 현재 볼륨 설정값을 UI에 반영함
    virtual void LoadCurrentSettings() override;
    // 현재 UI의 값을 Config에 저장
    virtual void ApplyCurrentSettings() override;
    // 설정 초기화
    virtual void ResetCurrentSettings() override;


    /* Sliders */
    UPROPERTY(meta = (BindWidget))
    USlider* MasterSlider;

    UPROPERTY(meta = (BindWidget))
    USlider* MusicSlider;

    UPROPERTY(meta = (BindWidget))
    USlider* SFXSlider;

    UPROPERTY(meta = (BindWidget))
    USlider* UISlider;

    /* TextBlock for Percent Values */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* MasterPercent;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* MusicPercent;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* SFXPercent;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* UIPercent;

private:
    /* Slider Event Functions */
    //UFUNCTION()
    //void OnVolumeChanged(float Value, EAudioCategory AudioType);

    /* Wrapper for OnVolumeChanged() */
    UFUNCTION()
    void OnMasterVolumeChanged(float Value);

    UFUNCTION()
    void OnMusicVolumeChanged(float Value);

    UFUNCTION()
    void OnSFXVolumeChanged(float Value);

    UFUNCTION()
    void OnUIVolumeChanged(float Value);

    // USlider* GetSliderByType(EAudioCategory Type) const;
    // UTextBlock* GetTextBlockByType(EAudioCategory Type) const;

    // 모든 오디오 타입의 OnValueChanged()를 바인딩함
    void BindSliderEvents();
    // 현재 볼륨값 업데이트
    void UpdatePercentageText(UTextBlock* TextWidget, float Value);
    // 슬라이더 업데이트
    // void UpdateSlidersFromSettings();
    void OnAudioSettingsChanged();

    bool bIsDelegateBound = false;
};