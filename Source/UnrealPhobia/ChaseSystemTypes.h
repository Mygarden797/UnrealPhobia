#pragma once

#include "CoreMinimal.h"
#include "ChaseSystemTypes.generated.h"

/**
*       Name				        : ChaseSystemTypes
*       Description		        : Manage Ingame Player State
*       LastUpate				: 2025/08/04
*       Todo                       :
*/

UENUM(BlueprintType)
enum class EChaseState : uint8
{
    Safe UMETA(DisplayName = "Safe"),                                       // Chasing is end
    BeingChased UMETA(DisplayName = "BeingChased"),          // Chasing now
    Cooldown UMETA(DisplayName = "Cooldown"),                  // Chasing is temporily end
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChaseStateSignature, EChaseState, NewState);
