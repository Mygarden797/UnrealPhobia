#include "SurvivorController.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "NetworkPlayer.h"

/*
ASurvivorController::ASurvivorController()
{
    PrimaryActorTick.bCanEverTick = false;
    bIsAudioSettingsVisible = false;
    bIsUIMode = false;
}

void ASurvivorController::BeginPlay()
{
    Super::BeginPlay();
}

void ASurvivorController::OnPossess(APawn* InCharacter)
{
    Super::OnPossess(InCharacter);
    SetupEnhancedInput();
    UE_LOG(LogTemp, Display, TEXT("ASurvivorController::OnPossess(): Possessed %s"),
        InCharacter ? *InCharacter->GetName() : TEXT("nullptr"));

    UE_LOG(LogTemp, Display, TEXT("Possessed Pawn Class: %s"), *InCharacter->GetClass()->GetName());

    if (InCharacter->IsA(ANetworkPlayer::StaticClass()))
    {
        ANetworkPlayer* MyNetworkPlayer = Cast<ANetworkPlayer>(InCharacter);
        if (MyNetworkPlayer)
        {
            MyNetworkPlayer->ReinitializeInputBindings();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OnPossess: Pawn is NOT ANetworkPlayer"));
    }

    EnableInput(this);
    if (InputComponent && InputComponent->IsActive())
    {
        UE_LOG(LogTemp, Display, TEXT("InputComponent is active"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("InputComponent is NOT active"));
    }
}

void ASurvivorController::OnUnPossess()
{
    if (bIsAudioSettingsVisible)
    {
        HideAudioSettings();
    }
    Super::OnUnPossess();
}

void ASurvivorController::SetupEnhancedInput()
{
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (SurvivorMovingContext)
        {
            Subsystem->AddMappingContext(SurvivorMovingContext, 1);
            UE_LOG(LogTemp, Display, TEXT("Moving Key is ready"));
        }
        else
        {
            UE_LOG(LogTemp, Error,
                TEXT("ASurvivorController::SetupEnhancedInput(): SurvivorMovingContext is null"));
        }
        if (UIMappingContext)
        {
            Subsystem->AddMappingContext(UIMappingContext, 0);
        }
        else
        {
            UE_LOG(LogTemp, Error,
                TEXT("ASurvivorController::SetupEnhancedInput(): UIMappingContext is null"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to Mapping Keys"));
    }
}

void ASurvivorController::CreateAudioSettingsWidget()
{
    if (!AudioSettingsWidget && AudioSettingsWidgetClass)
    {
        AudioSettingsWidget = CreateWidget<UGameAudioSettingsWidget>
            (this, AudioSettingsWidgetClass);
    }
    else
    {
        UE_LOG(LogTemp, Error,
            TEXT("ASurvivorController::CreateAudioSettingsWidget(): Failed to create Widgets"));
    }
}

void ASurvivorController::ToggleAudioSettings(const FInputActionValue& Value)
{
    const bool bPressed = Value.Get<bool>();
    if (bPressed)
    {
        UE_LOG(LogTemp, Display, TEXT("SurvivorController::ToggleAudioSettings(): Key is pressed"));
        if (bIsAudioSettingsVisible)
        {
            HideAudioSettings();
        }
        else
        {
            ShowAudioSettings();
        }
    }
}

void ASurvivorController::ShowAudioSettings()
{
    if (!AudioSettingsWidget)
    {
        UE_LOG(LogTemp, Error,
            TEXT("AMyPlayerController::ShowAudioSettings(): AudioSettingsWidget is null"));
        return;
    }

    AudioSettingsWidget->AddToViewport();
    SetInputMode(FInputModeUIOnly());
    SetShowMouseCursor(true);

    bIsUIMode = true;
    bIsAudioSettingsVisible = true;
}

void ASurvivorController::HideAudioSettings()
{
    if (!AudioSettingsWidget)
    {
        UE_LOG(LogTemp, Error,
            TEXT("AMyPlayerController::HideAudioSettings(): AudioSettingsWidget is null"));
        return;
    }

    AudioSettingsWidget->RemoveFromParent();
    SetInputMode(FInputModeGameOnly());
    SetShowMouseCursor(false);

    bIsUIMode = false;
    bIsAudioSettingsVisible = false;
}
*/