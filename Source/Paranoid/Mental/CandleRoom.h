#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Mental/Candle.h"
#include "CandleRoom.generated.h"

class UBoxComponent;

UCLASS()
class PARANOID_API ACandleRoom : public AActor
{
	GENERATED_BODY()

public:
	ACandleRoom();

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "CandleRoom")
	TArray<TObjectPtr<ACandle>> AssociatedCandles;

	void TurnOnEffects();

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
