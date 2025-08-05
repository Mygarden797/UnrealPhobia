#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ChaseSystemTypes.h"
#include "NetworkPlayer.h"
#include "InputActionValue.h"
#include "Managers/SoundManager.h"
#include "SurvivorController.generated.h"

/**
*       Name				        : SurvivorController
*       Description		        : PlayerController for Ingame
*       LastUpate				: 2025/07/29
*       Todo                       :
*/

UCLASS(Blueprintable)
class UNREALPHOBIA_API ASurvivorController : public APlayerController
{
	GENERATED_BODY()

    // virtual void OnPossess(APawn* InCharacter) override;

public:
 //   UFUNCTION(BlueprintCallable)
 //   void ToggleInGameMenu();
    

//protected:
 //   virtual void SetupInputComponent() override;

private:
 //   UPROPERTY()
 //   UUserWidget* CurrentInGameMenu;
        
 //   void OnEscapePressed();
    bool bIsMenuOpen = false;
};