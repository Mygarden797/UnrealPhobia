#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/StreamableManager.h"
#include "Sound/SoundMix.h"
#include "DynamicOcclusionComponent.generated.h"

/**
*       Name				        : DynamicOcclusionComponent
*       Description		        : Make Sound Effect
*       LastUpate				: 2025/08/13
*       Todo                       :
*/

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPHOBIA_API UDynamicOcclusionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDynamicOcclusionComponent();
    virtual void InitializeComponent() override;

protected:
    virtual void BeginPlay() override;


    UFUNCTION()
    void CheckOcclusion();

private:
    // Tick Interval
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Occlusion Settings", meta = (AllowPrivateAccess = "true"))
    float UpdateInterval;

    // Audio Tracing Channel
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Occlusion Settings", meta = (AllowPrivateAccess = "true"))
    TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

    // Drawing line for Debug
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Occlusion Settings", meta = (AllowPrivateAccess = "true"))
    bool bDrawDebug = true;

    /* Sound Mix for Occlusion */
    // Sound Mix의 경로
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings", meta = (AllowPrivateAccess = "true"))
    TSoftObjectPtr<USoundMix> OcclusionSoundMix;
    // 실제 Sound Mix
    UPROPERTY()
    TObjectPtr<USoundMix> LoadedOcclusionSoundMix;
    
    // Async loader
    FStreamableManager StreamableManager;

    FTimerHandle OcclusionTimerHandle;
    bool bIsOccluded = false;
};
