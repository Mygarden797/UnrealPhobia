#include "AudioAssets.h"
#include "Engine/Blueprint.h"
#include "Engine/Engine.h"
#include "UObject/SoftObjectPath.h"

UAudioAssets* UAudioAssets::CachedAudioAssets = nullptr;

UAudioAssets* UAudioAssets::LoadAudioAssets()
{
    if (CachedAudioAssets)
    {
        return CachedAudioAssets;
    }

    FString AudioAssetPath;
    if (GConfig && GConfig->GetString(TEXT("GameAudio"), TEXT("DA_AudioAssetPath"), AudioAssetPath, GGameIni))
    {
        if (!AudioAssetPath.IsEmpty())
        {
            FSoftObjectPath AudioDataPath(AudioAssetPath);

            // 먼저 DataAsset으로 직접 로드 시도
            CachedAudioAssets = Cast<UAudioAssets>(AudioDataPath.TryLoad());

            // 실패시 Blueprint로 로드
            // 현 프로젝트에서는 블루프린트로 동작할 예정
            if (!CachedAudioAssets)
            {
                UBlueprint* LoadedBlueprint = Cast<UBlueprint>(AudioDataPath.TryLoad());
                if (LoadedBlueprint && LoadedBlueprint->GeneratedClass)
                {
                    CachedAudioAssets = Cast<UAudioAssets>(LoadedBlueprint->GeneratedClass->GetDefaultObject());
                }
            }
        }
    }

    if (!CachedAudioAssets)
    {
        UE_LOG(LogTemp, Error, 
            TEXT("AudioAssets::LoadAudioAssets(): Failed to load AudioAssets from path: %s"), 
            *AudioAssetPath);
    }

    return CachedAudioAssets;
}

