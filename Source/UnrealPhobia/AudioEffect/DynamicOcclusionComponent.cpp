#include "AudioEffect/DynamicOcclusionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"

UDynamicOcclusionComponent::UDynamicOcclusionComponent()
{
    // Disable Tick
    PrimaryComponentTick.bCanEverTick = false;
    UpdateInterval = 0.1f;
}

void UDynamicOcclusionComponent::InitializeComponent()
{
    Super::InitializeComponent();

    if (OcclusionSoundMix.ToSoftObjectPath().IsValid())
    {
        StreamableManager.RequestAsyncLoad(OcclusionSoundMix.ToSoftObjectPath(), [this]()
            {
                LoadedOcclusionSoundMix = OcclusionSoundMix.Get();
                if (!LoadedOcclusionSoundMix)
                {
                    UE_LOG(LogTemp, Error, TEXT("UDynamicOcclusionComponent::InitializeComponent(): Failed to load %s"), 
                        *OcclusionSoundMix.ToString());
                }
            });
    }
}


void UDynamicOcclusionComponent::BeginPlay()
{
    Super::BeginPlay();

    GetWorld()->GetTimerManager().SetTimer(
        OcclusionTimerHandle,
        this,
        &UDynamicOcclusionComponent::CheckOcclusion,
        UpdateInterval,
        true
    );
}

void UDynamicOcclusionComponent::CheckOcclusion()
{
    APlayerCameraManager* CameraManager =
        UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);

    if (!CameraManager)
    {
        UE_LOG(LogTemp, Error, TEXT("UDynamicOcclusionComponent::CheckOcclusion(): Failed to get CameraManager"));
        return;
    }

    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
    {
        UE_LOG(LogTemp, Error, TEXT("UDynamicOcclusionComponent::CheckOcclusion(): Failed to get OwnerActor"));
        return;
    }

    FVector SoundLocation = OwnerActor->GetActorLocation();
    FVector ListenerLocation = CameraManager->GetCameraLocation();
    FHitResult HitResult;

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        SoundLocation,
        ListenerLocation,
        TraceChannel
    );

    if (bDrawDebug)
    {
        FColor LineColor = bHit ? FColor::Red : FColor::Green;
        DrawDebugLine(GetWorld(), SoundLocation, ListenerLocation, LineColor, false, UpdateInterval, 0, 1.0f);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UDynamicOcclusionComponent::CheckOcclusion(): bDrawDebug is false"));
    }

    if (bHit && !bIsOccluded)
    {
        bIsOccluded = true;
        UGameplayStatics::PushSoundMixModifier(GetWorld(), LoadedOcclusionSoundMix);
    }
    else
    {
        bIsOccluded = false;
        UGameplayStatics::PopSoundMixModifier(GetWorld(), LoadedOcclusionSoundMix);
    }
}

