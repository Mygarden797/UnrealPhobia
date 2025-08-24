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
            CachedAudioAssets = Cast<UAudioAssets>(AudioDataPath.TryLoad());
        }
    }

    if (!CachedAudioAssets)
    {
        UE_LOG(LogTemp, Error,
            TEXT("AudioAssets::LoadAudioAssets(): Failed to load AudioAssets from path: %s"),
            *AudioAssetPath);
        return nullptr;
    }

    return CachedAudioAssets;
}