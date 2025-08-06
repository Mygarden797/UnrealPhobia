#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Mental/Candle.h"
#include "CandleRoom.generated.h"

class UBoxComponent;

UCLASS()
class UNREALPHOBIA_API ACandleRoom : public AActor
{
	GENERATED_BODY()

public:
	ACandleRoom();

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "CandleRoom")
	TArray<TObjectPtr<ACandle>> AssociatedCandles;

	/** 이 방의 불꽃 등 활성화 효과를 켭니다. */
	void TurnOnEffects();

	/** 이 방의 불꽃 등 활성화 효과를 끕니다. */
	void TurnOffEffects();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Trigger")
	UBoxComponent* TriggerBox;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                    const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, Category = "Mental Regen")
	float RegenPerTick = 6.f;

	UPROPERTY(EditAnywhere, Category = "Mental Regen")
	float RegenInterval = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Mental Regen")
	float RegenTotalAmount = 60.f;
};
