#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Candle.generated.h"

UCLASS()
class PARANOID_API ACandle : public AActor
{
    GENERATED_BODY()

public:
    ACandle();

protected:
    virtual void BeginPlay() override;

public:
    void SetFlameActive(bool bIsActive);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
    TObjectPtr<UStaticMeshComponent> CandleMeshComponent;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Materials")
    TObjectPtr<UMaterialInterface> FlameOnMaterial;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Materials")
    TObjectPtr<UMaterialInterface> FlameOffMaterial;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Materials")
    int32 FlameMaterialSlotIndex = 1;
};
