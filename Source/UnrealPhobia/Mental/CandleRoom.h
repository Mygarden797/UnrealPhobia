#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CandleRoom.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class UNREALPHOBIA_API ACandleRoom : public AActor
{
	GENERATED_BODY()

public:
	ACandleRoom();
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CubeMesh;
	
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

	// 회복 수치 설정
	UPROPERTY(EditAnywhere, Category = "Mental Regen")
	float RegenPerTick = 6.f;

	UPROPERTY(EditAnywhere, Category = "Mental Regen")
	float RegenInterval = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Mental Regen")
	float RegenTotalAmount = 60.f;

public:

};
