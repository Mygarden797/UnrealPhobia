#include "SurvivorController.h"
#include "NetworkPlayer.h"
#include "UnrealPhobia/Managers/SoundManager.h"

void ASurvivorController::OnPossess(APawn* InCharacter)
{
    Super::OnPossess(InCharacter);

    ANetworkPlayer* MySurvivor = Cast<ANetworkPlayer>(GetCharacter());
    if (!MySurvivor)
    {
        UE_LOG(LogTemp, Error, TEXT("ASurvivorController: Possessed Pawn is not NetworkPlayer!"));
    }

    if (!SoundManager)
    {
        if (UGameInstance* GameInstance = GetGameInstance())
        {
            SoundManager = GameInstance->GetSubsystem<USoundManager>();
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ASurvivorController: Failed to get SoundManager"));
    }

    MySurvivor->OnChaseStateChanged.AddDynamic(this, &ASurvivorController::UpdateChaseState);
}

/*
void ASurvivorController::UpdateChaseState(EChaseState NewState)
{
    if (SoundManager)
    {
        SoundManager->PlayBeingChased(NewState);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to call SoundManager!!"));
    }
}
*/