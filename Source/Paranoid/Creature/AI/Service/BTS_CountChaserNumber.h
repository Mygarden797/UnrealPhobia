#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "NetworkPlayer.h"
#include "BTS_CountChaserNumber.generated.h"

/**
*       Name				        : CountChaseNumber
*       Description		        : Manage number of creatures chasing player characters
*       Last Update		    : 2025/08/04
*       Todo                       : 
*/
UCLASS()
class PARANOID_API UBTS_CountChaserNumber : public UBTService
{
    GENERATED_BODY()

public:
    UBTS_CountChaserNumber();

protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
    virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetKey;

private:
    UPROPERTY()
    TWeakObjectPtr<ANetworkPlayer> LastChasedPlayer;
};
